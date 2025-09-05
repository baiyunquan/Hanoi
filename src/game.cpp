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

// Game-related State data
SpriteRenderer    *Renderer;
ParticleGenerator* Particles;
PostProcessor* Effects;
ISoundEngine* SoundEngine;
TextRenderer* Text;

float ShakeTime = 0.0f;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_MENU), Keys(),KeysProcessed(), Width(width), Height(height) , Level(0), Lives(3),Points(0)
{}

Game::~Game()
{

}

void Game::Init()
{
    SoundEngine = createIrrKlangDevice();

    // Load shaders
    ResourceManager::LoadShader("shaders/sprite/vertShader.glsl", "shaders/sprite/fragShader.glsl", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/particles/vertShader.glsl", "shaders/particles/fragShader.glsl", nullptr, "particle");
    ResourceManager::LoadShader("shaders/post_processor/vertShader.glsl", "shaders/post_processor/fragShader.glsl", nullptr, "post_processor");
    ResourceManager::LoadShader("shaders/rectangle/vertShader.glsl", "shaders/rectangle/fragShader.glsl", nullptr, "rectangle");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
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


void Game::Render()
{
    
}
