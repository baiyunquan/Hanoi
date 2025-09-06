/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "game_object.h"


GameObject::GameObject()
    : Position(0, 0), SizeX(1.0f) , SizeY(1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), isRect(false) {
}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), SizeX(size.x), SizeY(size.y), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), isRect(false) {
}

GameObject::GameObject(glm::vec2 pos, float sizeX, float sizeY, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), SizeX(sizeX), SizeY(sizeY), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(), isRect(true) {
}

void GameObject::Draw(SpriteRenderer& renderer)
{
    if (isRect) {
        renderer.DrawRectangle(this->Position, glm::vec2(this->SizeX , this->SizeY), this->Rotation, this->Color);
		return;
    }
    renderer.DrawSprite(this->Sprite, this->Position, glm::vec2(this->SizeX, this->SizeY), this->Rotation, this->Color);
}

void GameObject::DrawText(TextRenderer& textRenderer)
{
    if (text.empty()) return;

    float textWidth = this->SizeX * 0.8;
    float textHeight = this->SizeY * 0.8;
    float xOffset = this->SizeX * 0.1;
    float yOffset = this->SizeY * 0.1;
    if (text.size() < 10) {
        textScale = 1.0f;
        if (textScale == 0) textScale = textRenderer.CalculateOptimalScale(text, textWidth, textHeight);
    }
    
    textRenderer.RenderTextInBox(text, this->Position.x + xOffset, this->Position.y + yOffset, textWidth, textHeight, textScale, glm::vec3(1.0, 1.0, 1.0));
}

bool GameObject::isChosen(float mouseX, float mouseY) {
    return mouseX >= Position.x && mouseX <= Position.x + SizeX &&
        mouseY >= Position.y && mouseY <= Position.y + SizeY;
}

std::string GameObject::getText()
{
    return std::string();
}

void GameObject::setText(std::string str)
{
    text = str;
    textScale = 0;
}
