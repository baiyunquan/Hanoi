#include "text_input.h"
#include <GLFW/glfw3.h>
#include <iostream>

TextInput::TextInput(float width, float height)
    : active(false), content("") , width(width) , height(height){


    float bgHeight = height * 0.3f;
    float bgWidth = width * 0.4f;
    float bgY = height * 0.4f;
    float bgX = width * 0.3f;

    // 创建背景块 - 使用半透明灰色
    background = new GameObject(glm::vec2(bgX, bgY), bgWidth, bgHeight,
        glm::vec3(0.5f, 0.5f, 0.5f));

    float inputHeight = height * 0.10f;
    float inputWidth = width * 0.3f;
    float inputY = height * 0.55f;
    float inputX = width * 0.35f;

    // 创建输入区域 - 使用白色背景
    inputArea = new GameObject(glm::vec2(inputX, inputY),
        inputWidth, inputHeight,
        glm::vec3(1.0f, 1.0f, 1.0f));

}

TextInput::~TextInput() {
    delete background;
    delete inputArea;
}

void TextInput::Draw(SpriteRenderer& spriteRenderer, TextRenderer& textRenderer) {
    if (!active) return;

    // 绘制背景
    background->Draw(spriteRenderer);

    // 绘制输入区域
    inputArea->Draw(spriteRenderer);

    textRenderer.RenderTextInBox("Enter Name(Press Enter to End)", width * 0.3f , height * 0.4f , width * 0.4f , height * 0.15f ,1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    // 绘制输入内容（在输入区域内）
    if (!content.empty()) {
        float textX = inputArea->Position.x + 5.0f; // 留一点边距
        float textY = inputArea->Position.y + (inputArea->SizeY - 20.0f) / 2.0f; // 垂直居中
        textRenderer.RenderTextInBox(content, width * 0.38f, height * 0.56f, width * 0.24f , height * 0.08f , 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void TextInput::ProcessKey(int key, int action) {
    if (!active) return;

    // 只在键释放时处理输入
    if (action == GLFW_RELEASE) {
        // 处理退格键
        if (key == GLFW_KEY_BACKSPACE && !content.empty()) {
            content.pop_back();
        }
        // 处理回车键
        else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
            if (onSubmitCallback) {
                onSubmitCallback(content);
            }
            active = false;
        }
        // 处理ESC键
        else if (key == GLFW_KEY_ESCAPE) {
            active = false;
        }
        // 处理可打印字符（ASCII范围32-126）
        else if (key >= 32 && key <= 126) {
            // 不处理大小写，直接转换为小写
            char c = static_cast<char>(key);
            if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a'; // 转换为小写
            }
            content += c;
        }
    }
}

std::string TextInput::getResult() {
    return content;
}

void TextInput::clear() {
    content.clear();
}

bool TextInput::isActive() const {
    return active;
}

void TextInput::setActive(bool active) {
    this->active = active;
    if (active) {
        clear();
    }
}

void TextInput::setOnSubmitCallback(std::function<void(const std::string&)> callback) {
    onSubmitCallback = callback;
}