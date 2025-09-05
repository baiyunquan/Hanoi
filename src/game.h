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

#include "game_level.h"

using namespace irrklang;

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};



class Game
{
public:
    // ��Ϸ״̬
    GameState  State;
    bool  Keys[1024];
    bool  KeysProcessed[1024];
    bool mousePressed = false;
    bool mouseWasPressed = false;
    unsigned int     Width, Height;

    std::vector<GameLevel> Levels;
    unsigned int            Level;
    unsigned int            Lives;
    int towerNum = 3;
	int towerLevel = 5;

    // ���캯��/��������
    Game(unsigned int width, unsigned int height);
    ~Game();
    // ��ʼ����Ϸ״̬���������е���ɫ��/����/�ؿ���
    void Init();
    // ��Ϸѭ��
    void ProcessInput(float dt);
    void ProcessMouse(float dt, GLFWwindow* window);
    void Update(float dt);
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