/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <algorithm>
#include <sstream>
#include <iostream>

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"
#include "step_manager.h"


// Game-related State data
SpriteRenderer    *Renderer;
ParticleGenerator* Particles;
PostProcessor* Effects;
ISoundEngine* SoundEngine;
TextRenderer* Text;
std::map<int , Hanoi*> towers;
StepManager stepManager;

float ShakeTime = 0.0f;
Move currentStep;
float displayTime = 0.0f;


Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(),KeysProcessed(), Width(width), Height(height) , Step(0)
{}

Game::~Game()
{

}

void Game::Init()
{
    SoundEngine = createIrrKlangDevice();

    float tenH = this->Height * 0.1f;
    float eightH = this->Height * 0.08f;

    float twentyW = this->Width * 0.2f;
    float twelfthW = this->Width * 0.12f;
    float twoW = this->Width * 0.02f;
    float sixW = this->Width * 0.06f;

	float towerWidth = (this->Width - twentyW) / (float)this->towerNum;
    float towerhHeight = this->Height - tenH;

    towers.emplace(0 ,new Hanoi(5, glm::vec2(twoW, eightH), glm::vec2(towerWidth, towerhHeight), false));
    for(int i = 1; i < this->towerNum; i++) {
        float posX = twoW + (i * towerWidth) + (sixW / (towerNum - 1)) * i;
        float posY = eightH;
        towers.emplace(i ,new Hanoi(towerLevel, glm::vec2(posX, posY), glm::vec2(towerWidth, towerhHeight), true));
	}

    topBarHeight = eightH;
    sideBarWidth = twelfthW;
    sideBarX = this->Width - sideBarWidth;

    // Load shaders
    ResourceManager::LoadShader("shaders/sprite/vertShader.glsl", "shaders/sprite/fragShader.glsl", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/particles/vertShader.glsl", "shaders/particles/fragShader.glsl", nullptr, "particle");
    ResourceManager::LoadShader("shaders/post_processor/vertShader.glsl", "shaders/post_processor/fragShader.glsl", nullptr, "post_processor");
    ResourceManager::LoadShader("shaders/rectangle/vertShader.glsl", "shaders/rectangle/fragShader.glsl", nullptr, "rectangle");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    ResourceManager::GetShader("rectangle").Use().SetMatrix4("projection", projection);
    // 加载纹理
    ResourceManager::LoadTexture("resources/textures/background.jpg", GL_FALSE, "background");

    // Set render-specific controls
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite") , ResourceManager::GetShader("rectangle"));
    Effects = new PostProcessor(ResourceManager::GetShader("post_processor"), this->Width, this->Height);


    SoundEngine->play2D("resources/audio/funky_stars.mp3", GL_TRUE);

    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("resources/fonts/arial.ttf", 24);
}

void Game::Update(float dt)
{
    if (displayTime > 0.0f) {
        displayTime -= dt;
    }
}


void Game::ProcessInput(float dt)
{
   
}

void Game::ProcessMouse(float dt, GLFWwindow* window) {
    // 检测是否为完整的鼠标点击（按下并释放）
    const bool isCompleteClick = (!mousePressed && mouseWasPressed);
    mouseWasPressed = mousePressed;

    if (!isCompleteClick) return;

    // 获取鼠标位置
    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

    // 尝试选择点击的盘子
    Plate* clickedPlate = nullptr;
    Hanoi* sourceTower = nullptr;

    for (auto& [towerId, tower] : towers) {
        if (tower->isEmpty()) continue;

        Plate& topPlate = tower->disks.begin()->second;
        if (topPlate.isChosen(cursorX, cursorY)) {
            // 取消其他塔顶盘子的选中状态
            clearOtherPlateSelections(towerId);

            topPlate.select();
            clickedPlate = &topPlate;
            sourceTower = tower;
            break;
        }
    }

    // 如果没有点击到盘子，检查柱子点击
    if (!clickedPlate) {
        handleTowerClick(cursorX, cursorY);
    }
}

// 清除其他塔的盘子选中状态
void Game::clearOtherPlateSelections(int currentTowerId) {
    for (auto& [towerId, tower] : towers) {
        if (towerId != currentTowerId && !tower->isEmpty()) {
            Plate& otherPlate = tower->disks.begin()->second;
            if (otherPlate.isSelect()) {
                otherPlate.select();
            }
        }
    }
}

// 处理塔点击事件
void Game::handleTowerClick(double cursorX, double cursorY) {
    // 查找当前选中的盘子
    Plate* selectedPlate = nullptr;

    int sourceId;
    Hanoi* sourceTower = nullptr;

    for (auto& [towerId, tower] : towers) {
        if (!tower->isEmpty()) {
            Plate& plate = tower->disks.begin()->second;
            if (plate.isSelect()) {
                selectedPlate = &plate;
                sourceTower = tower;
                sourceId = towerId;
                break;
            }
        }
    }

    if (!selectedPlate) return;

    // 检查是否点击了目标塔
    for (auto& [towerId, targetTower] : towers) {
        if (targetTower->pole.isChosen(cursorX, cursorY)) {
            // 验证移动是否合法
            if (isMoveValid(*targetTower, *selectedPlate)) {
                movePlate(*sourceTower,sourceId ,*targetTower , towerId);
            }
            break;
        }
    }

}

// 检查移动是否合法
bool Game::isMoveValid(Hanoi& targetTower, Plate& plate){
    return targetTower.isEmpty() || targetTower.getTop() > plate.level;
}

// 辅助函数：执行盘子移动
void Game::movePlate(Hanoi& sourceTower,int sourceId ,  Hanoi& targetTower , int targetId) {
    auto [plateLevel, plateObj] = sourceTower.PopTop();

    // 更新盘子位置
    const float xOffset = targetTower.pos.x - sourceTower.pos.x;
    plateObj.Position.x += xOffset;

    currentStep.from = sourceId ;
    currentStep.to = targetId;
    displayTime = 10.0f;
    Step++;

    plateObj.select(); // 取消选中状态
    targetTower.PushTop(plateObj, plateLevel);
}

void Game::Render()
{
    // Render towers
    for (auto& [num, tower] : towers) {
		tower->Draw(*Renderer , *Text);
    }

    // Render sidebar , topbar
    std::string sbText{"Step: "};
    sbText.append(std::to_string(Step));
    if (displayTime > 0.0f) {
        sbText += "      Switch Tower " + std::to_string(currentStep.from) + " To " + std::to_string(currentStep.to);
    }
    Text->RenderTextInBox(sbText, 0, 0, this->Width, topBarHeight, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}
