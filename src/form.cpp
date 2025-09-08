#include "form.h"
#include <glm/glm.hpp>

void Header::Render(SpriteRenderer& renderer, TextRenderer& textRenderer,float height, float keyWidth, float valueWidth, glm::vec2 pos) {
    // �����ı�λ�ã�������ʾ��
     textRenderer.RenderTextInBox(key, pos.x, pos.y,keyWidth , height, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    float valueColWidth = valueWidth / value.size();
    for (size_t i = 0; i < value.size(); i++) {
        textRenderer.RenderTextInBox(value[i], pos.x + keyWidth + (valueColWidth * i), pos.y, valueColWidth , height, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // �ڵײ���һ����
    renderer.DrawLine(glm::vec2(pos.x, pos.y + height - 2.0f), glm::vec2(pos.x + keyWidth + (valueWidth * (value.size() - 1)), pos.y + height - 2.0f), 2.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void Line::Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float height, float keyWidth, float valueWidth, glm::vec2 pos) {
    // ʹ��TextRenderer��Ⱦkey�ı�
    textRenderer.RenderTextInBox(key, pos.x, pos.y, keyWidth, height, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    // ʹ��GameObject��Ⱦvalue��
    float valueColWidth = valueWidth / values.size();
    for (size_t i = 0; i < values.size(); i++) {
        // ����ԭʼ��Ԫ��λ�úʹ�С
        glm::vec2 originalPos(pos.x + keyWidth + i * valueColWidth, pos.y);
        glm::vec2 originalSize(valueColWidth, height);

        // ����80%�����Ĵ�С
        glm::vec2 scaledSize = originalSize * 0.8f;

        // �������λ�ã��������ĵ㲻�䣩
        glm::vec2 centeredPos = originalPos + (originalSize - scaledSize) * 0.5f;

        // ����GameObject��λ�úʹ�С
        values[i].Position = centeredPos;
        values[i].SizeX = scaledSize.x;
        values[i].SizeY = scaledSize.y;

        // ��ȾGameObject
        values[i].Draw(renderer);
        values[i].DrawText(textRenderer);
    }
}

// Form��ʵ��
Form::Form(std::string keyName, std::vector<std::string> valueName) :
    header(Header(keyName, valueName)) {}

void Form::updateData(const std::vector<std::string>& data) {
    // ֻ��Ӳ����ڵ�keyֵ
    for (const auto& key : data) {
        // ���key�Ƿ��Ѵ���
        bool exists = false;
        for (const auto& line : lines) {
            if (line.key == key) {
                exists = true;
                break;
            }
        }

        // ���key�����ڣ����������
        if (!exists) {
            std::vector<GameObject> values;
            for (size_t j = 0; j < header.value.size(); j++) {
                GameObject cell(glm::vec2(0, 0), 0, 0, glm::vec3(0.8f, 0.8f, 0.8f));
                cell.setText(header.value[j]); // ��ʼֵΪ��
                values.push_back(cell);
            }

            lines.push_back(Line(key, values));
        }
    }
}


void Form::Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float screenWidth, float screenHeight) {
    // ������ߴ��λ��
    float margin = 20.0f;
    float headerHeight = 60.0f;
    float rowHeight = 50.0f;
    float keyWidth = screenWidth / 5 - margin;
    float valueWidth = screenWidth * 4 / 5 - margin;
    glm::vec2 startPos(margin, margin);

    // ��Ⱦ��ͷ
    header.Render(renderer, textRenderer, headerHeight, keyWidth, valueWidth, startPos);

    // ��Ⱦ������
    for (size_t i = 0; i < lines.size(); i++) {
        glm::vec2 rowPos(startPos.x, startPos.y + headerHeight + i * rowHeight);
        lines[i].Render(renderer, textRenderer, rowHeight, keyWidth, valueWidth, rowPos);
    }
}