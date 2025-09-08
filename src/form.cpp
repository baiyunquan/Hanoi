// form.cpp 修改后
#include "form.h"
#include <glm/glm.hpp>

void Header::Render(SpriteRenderer& renderer, TextRenderer& textRenderer,float height, float keyWidth, float valueWidth, glm::vec2 pos) {
    // 计算文本位置（居中显示）
    textRenderer.RenderText(key, pos.x, pos.y, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    float valueColWidth = valueWidth / value.size();
    for (size_t i = 0; i < value.size(); i++) {
        textRenderer.RenderText(value[i], pos.x + keyWidth + (valueColWidth * i), pos.y, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));
    }

    // 在底部画一条线
    renderer.DrawLine(glm::vec2(pos.x, pos.y + height - 2.0f), glm::vec2(pos.x + keyWidth + (valueWidth * i), pos.y + height - 2.0f), 2.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void Line::Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float height, float keyWidth, float valueWidth, glm::vec2 pos) {
    // 使用TextRenderer渲染key文本
    textRenderer.RenderText(key, pos.x, pos.x, 1.0f, glm::vec3(0.0f, 0.0f, 0.0f));

    // 使用GameObject渲染value列
    float valueColWidth = valueWidth / values.size();
    for (size_t i = 0; i < values.size(); i++) {
        values[i].Position = glm::vec2(pos.x + keyWidth + i * valueColWidth, pos.y);
        values[i].SizeX = valueColWidth;
        values[i].SizeY = height;
        values[i].Draw(renderer);
        values[i].DrawText(textRenderer);
    }
}