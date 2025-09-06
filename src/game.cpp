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
#include "hanoi.h"

// Game-related State data
SpriteRenderer    *Renderer;
ParticleGenerator* Particles;
PostProcessor* Effects;
ISoundEngine* SoundEngine;
TextRenderer* Text;
std::vector<Hanoi*> towers;

float ShakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(),KeysProcessed(), Width(width), Height(height) , Level(0)
{}

Game::~Game()
{

}

void Game::Init()
{
    SoundEngine = createIrrKlangDevice();

	float towerWidth = (this->Width - 200.0f) / (float)this->towerNum;
    float towerhHeight = this->Height - 100.0f;

    towers.push_back(new Hanoi(5, glm::vec2(50.0f, 50.0f), glm::vec2(towerWidth, towerhHeight), false));
    for(int i = 1; i < this->towerNum; i++) {
        float posX = 50.0f + (i * towerWidth) + (100.0f / (towerNum - 1)) * i;
        float posY = 50.0f;
        towers.push_back(new Hanoi(towerLevel, glm::vec2(posX, posY), glm::vec2(towerWidth, towerhHeight), true));
	}

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
    // 加载关卡
    GameLevel one; one.Load("resources/levels/one.lvl", this->Width, this->Height * 0.5);
    GameLevel two; two.Load("resources/levels/two.lvl", this->Width, this->Height * 0.5);
    GameLevel three; three.Load("resources/levels/three.lvl", this->Width, this->Height * 0.5);
    GameLevel four; four.Load("resources/levels/four.lvl", this->Width, this->Height * 0.5);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);
    this->Levels.push_back(four);
    this->Level = 0;

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
    
}


void Game::ProcessInput(float dt)
{
   
}

void Game::ProcessMouse(float dt, GLFWwindow* window) {
    // 检测鼠标点击（按下并释放）
    bool mouseClicked = (!mousePressed && mouseWasPressed);
    mouseWasPressed = mousePressed;

    if (!mouseClicked) return;

    // 获取鼠标位置
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // 首先检查是否点击了某个盘子
    Plate* selectedPlate = nullptr;
    Hanoi* sourceTower = nullptr;

    for (auto& tower : towers) {
        if (!tower->isEmpty()) {
            Plate& plate = tower->disks.begin()->second;
            if (plate.isChosen(xpos, ypos)) {
                // 取消其他盘子的选中状态
                for (auto& otherTower : towers) {
                    if (!otherTower->isEmpty()) {
                        Plate& otherPlate = otherTower->disks.begin()->second;
                        if (otherPlate.isSelect() && &otherPlate != &plate) {
                            otherPlate.select(); // 取消选中
                        }
                    }
                }

                plate.select(); // 切换选中状态
                selectedPlate = &plate;
                sourceTower = tower;
                break;
            }
        }
    }

    // 如果没有选中盘子，检查是否点击了柱子（用于移动已选中的盘子）
    if (!selectedPlate) {
        // 查找当前选中的盘子
        for (auto& tower : towers) {
            if (!tower->isEmpty()) {
                Plate& plate = tower->disks.begin()->second;
                if (plate.isSelect()) {
                    selectedPlate = &plate;
                    sourceTower = tower;
                    break;
                }
            }
        }

        // 如果有选中的盘子，检查是否点击了目标柱子
        if (selectedPlate) {
            for (auto& targetTower : towers) {
                if (targetTower->pole.isChosen(xpos, ypos)) {
                    // 检查移动是否合法
                    if (targetTower->isEmpty() || targetTower->getTop() > selectedPlate->level) {
                        // 执行移动
                        std::pair<int, Plate> temp = sourceTower->PopTop();
                        temp.second.Position.x += targetTower->pos.x - sourceTower->pos.x;
                        temp.second.select(); // 取消选中
                        targetTower->PushTop(temp.second, temp.first);
                    }
                    break;
                }
            }
        }
    }
}


void Game::Render()
{
    for (auto& tower : towers) {
		tower->Draw(*Renderer);
    }
}
