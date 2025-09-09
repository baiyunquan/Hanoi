/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef GAME_H
#define GAME_H
#include <iostream>
#include <vector>
#include <tuple>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <irrKlang.h>

#include "hanoi.h"
#include "text_input.h"
#include "messagebox.h"
#include "timer.h"

using namespace irrklang;

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_LOAD,
    GAME_SWITCH,
};

class Game
{
public:
    // 游戏状态
    GameState  State;
    bool  Keys[1024];
    bool  KeysProcessed[1024];
    bool mousePressed = false;
    bool mouseWasPressed = false;
    bool isTextInputMode = false;

    unsigned int     Width, Height;
    int topBarHeight;
    int sideBarWidth , sideBarHeight;
    int sideBarX;

    std::string switchTemp;

    TextInput* textInput;
    // 创建消息框
    MessageBox* messageBox;
    Timer timer;

    unsigned int     Step;
    int towerNum = 3;
	int towerLevel = 5;

    // 构造函数/析构函数
    Game(unsigned int width, unsigned int height);
    ~Game();
    // 初始化游戏状态（加载所有的着色器/纹理/关卡）
    void Init();
    void enter();
    bool beginRecord(std::string name);
    // 游戏循环
    void ProcessInput(float dt);
    void ProcessMouse(float dt, GLFWwindow* window);
    void clearOtherPlateSelections(int currentTowerId);
    void handleTowerClick(double cursorX, double cursorY);
    bool isMoveValid(Hanoi& targetTower, Plate& plate);
    bool isMoveValid(Hanoi* sourceTower, Hanoi* targetTower);
    void Update(float dt);
    void movePlate(Hanoi& sourceTower, int sourceId, Hanoi& targetTower, int targetId);
    void Render();

    void ResetLevel();
};

#endif


/*#ifndef glCheckError()
GLenum glCheckError_(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }
        std::cout << error << " | " << file << " (" << line << ")" << std::endl;
    }
    return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__) 

#endif // glCheckError()*/