#include "form.h"
#include <glm/glm.hpp>

void Header::Render(SpriteRenderer& renderer, TextRenderer& textRenderer,float height, float keyWidth, float valueWidth, glm::vec2 pos) {
    // 计算文本位置（居中显示）
     textRenderer.RenderTextInBox(key, pos.x, pos.y,keyWidth , height, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));

    float valueColWidth = valueWidth / value.size();
    for (size_t i = 0; i < value.size(); i++) {
        textRenderer.RenderTextInBox(value[i], pos.x + keyWidth + (valueColWidth * i), pos.y, valueColWidth , height, 1.0f, glm::vec3(0.0f, 0.0f, 1.0f));
    }

    // 在底部画一条线
    renderer.DrawLine(glm::vec2(pos.x, pos.y + height - 2.0f), glm::vec2(pos.x + keyWidth + (valueWidth * (value.size() - 1)), pos.y + height - 2.0f), 2.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void Line::Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float height, float keyWidth, float valueWidth, glm::vec2 pos) {
    // 使用TextRenderer渲染key文本
    textRenderer.RenderTextInBox(key, pos.x, pos.y, keyWidth, height, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f));

    // 使用GameObject渲染value列
    float valueColWidth = valueWidth / values.size();
    for (size_t i = 0; i < values.size(); i++) {
        // 计算原始单元格位置和大小
        glm::vec2 originalPos(pos.x + keyWidth + i * valueColWidth, pos.y);
        glm::vec2 originalSize(valueColWidth, height);

        // 计算80%比例的大小
        glm::vec2 scaledSize = originalSize * 0.8f;

        // 计算居中位置（保持中心点不变）
        glm::vec2 centeredPos = originalPos + (originalSize - scaledSize) * 0.5f;

        // 设置GameObject的位置和大小
        values[i].Position = centeredPos;
        values[i].SizeX = scaledSize.x;
        values[i].SizeY = scaledSize.y;

        // 渲染GameObject
        values[i].Draw(renderer);
        values[i].DrawText(textRenderer);
    }
}

// Form类实现
Form::Form(std::string keyName, std::vector<std::string> valueName) :
    header(Header(keyName, valueName)) {}

void Form::updateData(const std::vector<std::string>& data) {
    // 只添加不存在的key值
    for (const auto& key : data) {
        // 检查key是否已存在
        bool exists = false;
        for (const auto& line : lines) {
            if (line.key == key) {
                exists = true;
                break;
            }
        }

        // 如果key不存在，则添加新行
        if (!exists) {
            std::vector<GameObject> values;
            for (size_t j = 0; j < header.value.size(); j++) {
                GameObject cell(glm::vec2(0, 0), 0, 0, glm::vec3(0.8f, 0.8f, 0.8f));
                cell.setText(header.value[j]); // 初始值为空
                values.push_back(cell);
            }

            lines.push_back(Line(key, values));
        }
    }
}


void Form::Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float screenWidth, float screenHeight) {
    // 计算表单尺寸和位置
    float margin = 20.0f;
    float headerHeight = 60.0f;
    float rowHeight = 50.0f;
    float keyWidth = screenWidth / 5 - margin;
    float valueWidth = screenWidth * 4 / 5 - margin;
    glm::vec2 startPos(margin, margin);

    // 渲染表头
    header.Render(renderer, textRenderer, headerHeight, keyWidth, valueWidth, startPos);

    // 渲染数据行
    for (size_t i = 0; i < lines.size(); i++) {
        glm::vec2 rowPos(startPos.x, startPos.y + headerHeight + i * rowHeight);
        lines[i].Render(renderer, textRenderer, rowHeight, keyWidth, valueWidth, rowPos);
    }
}