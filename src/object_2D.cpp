#include "object_2d.h"


Object2D::Object2D()
    : Position(0, 0), Width(1.0f) , Height(1.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), isRect(false) {
}

Object2D::Object2D(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Width(size.x), Height(size.y), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), isRect(false) {
}

Object2D::Object2D(glm::vec2 pos, float sizeX, float sizeY, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Width(sizeX), Height(sizeY), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(), isRect(true) {
}

void Object2D::Draw(SpriteRenderer& renderer)
{
    if (isRect) {
        renderer.DrawRectangle(this->Position, glm::vec2(this->Width , this->Height), this->Rotation, this->Color);
		return;
    }
    renderer.DrawSprite(this->Sprite, this->Position, glm::vec2(this->Width, this->Height), this->Rotation, this->Color);
}

void Object2D::DrawText(TextRenderer& textRenderer)
{
    if (text.empty()) return;

    float textWidth = this->Width * 0.8f;
    float textHeight = this->Height * 0.8f;
    float xOffset = this->Width * 0.1f;
    float yOffset = this->Height * 0.1f;
    if (text.size() < 10) {
        textScale = 1.5f;
    }
    else {
        textScale = textRenderer.CalculateOptimalScale(text, textWidth, textHeight);
    }
    
    if (this->Color.x == 1.0f && this->Color.y == 1.0f && this->Color.z == 1.0f) {
        textRenderer.RenderTextInBox(text, this->Position.x + xOffset, this->Position.y + yOffset, textWidth, textHeight, textScale, glm::vec3(0.0f));
    } else {
        textRenderer.RenderTextInBox(text, this->Position.x + xOffset, this->Position.y + yOffset, textWidth, textHeight, textScale, glm::vec3(1.0f));
    }
}

bool Object2D::isChosen(float mouseX, float mouseY) {
    return mouseX >= Position.x && mouseX <= Position.x + Width &&
        mouseY >= Position.y && mouseY <= Position.y + Height;
}

std::string Object2D::getText()
{
    return std::string();
}

void Object2D::setText(std::string str)
{
    text = str;
}
