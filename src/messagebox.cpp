#include "messagebox.h"
#include <GLFW/glfw3.h>
#include <iostream>

MessageBox::MessageBox(float width, float height)
    : active(false), message(""), width(width), height(height) {

    // ���������� - ʹ�ð�͸����ɫ
    float bgHeight = height * 0.4f;
    float bgWidth = width * 0.5f;
    float bgY = height * 0.3f;
    float bgX = width * 0.25f;

    background = new GameObject(glm::vec2(bgX, bgY), bgWidth, bgHeight,
        glm::vec3(0.5f, 0.5f, 0.5f));

    // ������ť���� - ʹ�ò�ͬ����ɫ
    float buttonWidth = width * 0.15f;
    float buttonHeight = height * 0.08f;
    float buttonX = width * 0.5f - buttonWidth / 2.0f;
    float buttonY = height * 0.5f;

    buttonArea = new GameObject(glm::vec2(buttonX, buttonY),
        buttonWidth, buttonHeight,
        glm::vec3(0.2f, 0.6f, 0.2f)); // ��ɫ��ť
    buttonArea->setText("OK");
}

MessageBox::~MessageBox() {
    delete background;
    delete buttonArea;
}

void MessageBox::Draw(SpriteRenderer& spriteRenderer, TextRenderer& textRenderer) {
    if (!active) return;

    // ���Ʊ���
    background->Draw(spriteRenderer);

    // ���ư�ť����
    buttonArea->Draw(spriteRenderer);

    // ������Ϣ�ı����ڱ��������ڣ�
    if (!message.empty()) {
        textRenderer.RenderTextInBox(message,
            width * 0.3f, height * 0.4f,
            width * 0.4f, height * 0.15f,
            1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // ���ư�ť�ı�
    buttonArea->DrawText(textRenderer);
}


void MessageBox::ProcessMouseClick(float x, float y) {
    if (!active) return;

    // ����Ƿ����˰�ť����
    if (buttonArea->isChosen(x, y)) {
        if (onConfirmCallback) {
            onConfirmCallback();
        }
        active = false;
    }
}

std::string MessageBox::getMessage() const {
    return message;
}

void MessageBox::setMessage(const std::string& message) {
    this->message = message;
}

bool MessageBox::isActive() const {
    return active;
}

void MessageBox::setActive(bool active) {
    this->active = active;
}

void MessageBox::setOnConfirmCallback(std::function<void()> callback) {
    onConfirmCallback = callback;
}