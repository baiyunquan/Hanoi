#include "text_input.h"
#include <GLFW/glfw3.h>
#include <iostream>

TextInput::TextInput(float width, float height)
    : active(false), content("") , width(width) , height(height){


    float bgHeight = height * 0.3f;
    float bgWidth = width * 0.4f;
    float bgY = height * 0.4f;
    float bgX = width * 0.3f;

    // ���������� - ʹ�ð�͸����ɫ
    background = new GameObject(glm::vec2(bgX, bgY), bgWidth, bgHeight,
        glm::vec3(0.5f, 0.5f, 0.5f));

    float inputHeight = height * 0.10f;
    float inputWidth = width * 0.3f;
    float inputY = height * 0.55f;
    float inputX = width * 0.35f;

    // ������������ - ʹ�ð�ɫ����
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

    // ���Ʊ���
    background->Draw(spriteRenderer);

    // ������������
    inputArea->Draw(spriteRenderer);

    textRenderer.RenderTextInBox("Enter Name(Press Enter to End)", width * 0.3f , height * 0.4f , width * 0.4f , height * 0.15f ,1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    // �����������ݣ������������ڣ�
    if (!content.empty()) {
        float textX = inputArea->Position.x + 5.0f; // ��һ��߾�
        float textY = inputArea->Position.y + (inputArea->SizeY - 20.0f) / 2.0f; // ��ֱ����
        textRenderer.RenderTextInBox(content, width * 0.38f, height * 0.56f, width * 0.24f , height * 0.08f , 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

void TextInput::ProcessKey(int key, int action) {
    if (!active) return;

    // ֻ�ڼ��ͷ�ʱ��������
    if (action == GLFW_RELEASE) {
        // �����˸��
        if (key == GLFW_KEY_BACKSPACE && !content.empty()) {
            content.pop_back();
        }
        // ����س���
        else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
            if (onSubmitCallback) {
                onSubmitCallback(content);
            }
            active = false;
        }
        // ����ESC��
        else if (key == GLFW_KEY_ESCAPE) {
            active = false;
        }
        // ����ɴ�ӡ�ַ���ASCII��Χ32-126��
        else if (key >= 32 && key <= 126) {
            // �������Сд��ֱ��ת��ΪСд
            char c = static_cast<char>(key);
            if (c >= 'A' && c <= 'Z') {
                c = c - 'A' + 'a'; // ת��ΪСд
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