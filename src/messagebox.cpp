#include "messagebox.h"
#include <GLFW/glfw3.h>
#include <iostream>

MessageBox::MessageBox(float width, float height)
    : active(false),hasTexture(false) , message(""), width(width), height(height) {

    // 创建背景块 - 使用半透明灰色
    float bgHeight = height * 0.4f;
    float bgWidth = width * 0.5f;
    float bgY = height * 0.3f;
    float bgX = width * 0.25f;

    background = new Object2D(glm::vec2(bgX, bgY), bgWidth, bgHeight,
        glm::vec3(0.5f, 0.5f, 0.5f));

    // 创建按钮区域 - 使用不同的颜色
    float buttonWidth = width * 0.15f;
    float buttonHeight = height * 0.08f;
    float buttonX = width * 0.5f - buttonWidth / 2.0f;
    float buttonY = bgY + bgHeight - buttonHeight * 1.5f;

    buttonArea = new Object2D(glm::vec2(buttonX, buttonY),
        buttonWidth, buttonHeight,
        glm::vec3(0.2f, 0.6f, 0.2f)); // 绿色按钮
    buttonArea->setText("OK");
}

MessageBox::~MessageBox() {
    delete background;
    delete buttonArea;
}

void MessageBox::Draw(SpriteRenderer& spriteRenderer, TextRenderer& textRenderer) {
    if (!isActive()) return;

    // 绘制背景
    background->Draw(spriteRenderer);

    // 绘制按钮区域
    buttonArea->Draw(spriteRenderer);

    if (hasTexture) {
        float textureX = width * 0.27f;
        float textureY = height * 0.35f;
        float textureWidth = width * 0.15f;
        float textureHeight = height * 0.3f;
        spriteRenderer.DrawSprite(sideTexture, glm::vec2(textureX, textureY), glm::vec2(textureWidth, textureHeight), 0.0f, glm::vec3(1.0f));
        if (!message.empty()) {
            textRenderer.RenderTextInBox(message,
                width * 0.43f, height * 0.35f,
                width * 0.3f, height * 0.2f,
                1.25f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }
    else {
        // 绘制消息文本（在背景区域内）
        if (!message.empty()) {
            textRenderer.RenderTextInBox(message,
                width * 0.3f, height * 0.4f,
                width * 0.4f, height * 0.15f,
                3.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }
    } 

    // 绘制按钮文本
    buttonArea->DrawText(textRenderer);
}


void MessageBox::ProcessMouseClick(float x, float y) {
    if (!isActive()) return;

    // 检查是否点击了按钮区域
    if (buttonArea->isChosen(x, y)) {
        if (onConfirmCallback) {
            onConfirmCallback();
        }
        active = false;
        hasTexture = false;
    }
}

std::string MessageBox::getMessage() const {
    return message;
}

void MessageBox::setMessage(const std::string message) {
    this->message = message;
    this->setActive(true);
}

void MessageBox::setMessage(const std::string message , const std::string texture) {
    this->message = message;
    this->setActive(true);
    Texture2D text = ResourceManager::GetTexture(texture);
    this->setTexture(text);
}

bool MessageBox::isActive() const {
    return active;
}

void MessageBox::setActive(bool active) {
    this->active = active;
}

void MessageBox::setTexture(Texture2D& texture)
{
    sideTexture = texture;
    hasTexture = true;
}

void MessageBox::setOnConfirmCallback(std::function<void()> callback) {
    onConfirmCallback = callback;
}