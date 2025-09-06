/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "text_renderer.h"
#include "resource_manager.h"


TextRenderer::TextRenderer(unsigned int width, unsigned int height)
{
    // load and configure shader
    this->TextShader = ResourceManager::LoadShader("shaders/text/vertShader.glsl", "shaders/text/fragShader.glsl", nullptr, "text");
    this->TextShader.SetMatrix4("projection", glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f), true);
    this->TextShader.SetInteger("text", 0);
    // configure VAO/VBO for texture quads
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TextRenderer::Load(std::string font, unsigned int fontSize)
{
    // first clear the previously loaded Characters
    this->Characters.clear();
    // then initialize and load the FreeType library
    FT_Library ft;    
    if (FT_Init_FreeType(&ft)) // all functions return a value different than 0 whenever an error occurred
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 
    // then for the first 128 ASCII characters, pre-load/compile their characters and store them
    for (GLubyte c = 0; c < 128; c++) // lol see what I did there 
    {
        // load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
            );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
       
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::RenderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state	
    this->TextShader.Use();
    this->TextShader.SetVector3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->VAO);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y + (this->Characters['H'].Bearing.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f },
            { xpos,     ypos,       0.0f, 0.0f },

            { xpos,     ypos + h,   0.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (1/64th times 2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}
float TextRenderer::calculateWidth(std::string text, float scale) {
    float width = 0.0f;

    // iterate through all characters
    for (char c : text) {
        Character ch = Characters[c];
        width += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels
    }

    return width;
}

float TextRenderer::calculateHeight(std::string text, float scale) {
    if (text.empty()) return 0.0f;

    float maxHeight = 0.0f;
    float minBearingY = FLT_MAX;

    // 查找文本中最高字符和最低的bearing.y
    for (char c : text) {
        Character ch = Characters[c];
        float height = ch.Size.y * scale;
        if (height > maxHeight) {
            maxHeight = height;
        }
        if (ch.Bearing.y < minBearingY) {
            minBearingY = ch.Bearing.y;
        }
    }

    // 考虑字符可能向下延伸的部分（如'g', 'p', 'q'等）
    // 总高度 = 最高字符的高度 + 向下延伸的部分
    return maxHeight + (Characters['H'].Bearing.y - minBearingY) * scale;
}

// 计算文本在指定宽度和高度下的最佳缩放比例
float TextRenderer::CalculateOptimalScale(const std::string& text,
    float availableWidth,
    float availableHeight) {
    if (text.empty() || availableWidth <= 0 || availableHeight <= 0) {
        return 1.0f;
    }

    // 初始缩放比例
    float scale = 1.0f;
    float minScale = 0.1f;
    float maxScale = 10.0f;

    // 使用二分查找法找到最佳缩放比例
    for (int i = 0; i < 10; i++) { // 迭代10次应该足够精确
        // 使用当前缩放比例分割文本
        std::vector<std::string> lines = SplitTextIntoLines(text, availableWidth, scale);

        // 计算文本块高度
        float textHeight = CalculateTextBlockHeight(lines, scale);

        // 检查是否适合可用高度
        if (textHeight <= availableHeight) {
            // 文本高度适合，尝试增大缩放比例
            minScale = scale;
            scale = (scale + maxScale) / 2.0f;
        }
        else {
            // 文本高度太大，减小缩放比例
            maxScale = scale;
            scale = (minScale + scale) / 2.0f;
        }
    }

    return scale;
}

// 自动计算缩放比例并在区域内渲染文本
void TextRenderer::RenderTextAutoScale(const std::string& text,
    float x, float y, float width, float height,
    glm::vec3 color) {
    if (text.empty() || width <= 0 || height <= 0) {
        return;
    }

    // 计算最佳缩放比例
    float optimalScale = 1.0;
    if (text.size() > 10) {
        optimalScale = CalculateOptimalScale(text, width, height);
    }

    // 使用最佳缩放比例渲染文本
    RenderTextInBox(text, x, y, width, height, optimalScale, color);
}

// 改进的文本块高度计算
float TextRenderer::CalculateTextBlockHeight(const std::vector<std::string>& lines, float scale) {
    if (lines.empty()) return 0.0f;

    // 使用字体的实际度量信息计算行高
    // 如果没有保存字体度量信息，使用字符'H'的高度作为参考
    float lineHeight;
    if (Characters.find('H') != Characters.end()) {
        // 行高 = 字符高度 + 一些额外间距
        lineHeight = (Characters['H'].Size.y + Characters['H'].Bearing.y * 0.2f) * scale;
    }
    else if (!Characters.empty()) {
        // 使用第一个可用字符
        Character firstChar = Characters.begin()->second;
        lineHeight = (firstChar.Size.y + firstChar.Bearing.y * 0.2f) * scale;
    }
    else {
        // 默认行高
        lineHeight = 20.0f * scale;
    }

    return lines.size() * lineHeight;
}

// 改进的文本分割算法
std::vector<std::string> TextRenderer::SplitTextIntoLines(const std::string& text, float maxWidth, float scale) {
    std::vector<std::string> lines;

    // 如果不需要换行（宽度足够），直接返回单行
    if (calculateWidth(text, scale) <= maxWidth) {
        lines.push_back(text);
        return lines;
    }

    std::istringstream stream(text);
    std::string word;
    std::string currentLine;

    while (stream >> word) {
        // 测试添加这个词后是否会超出宽度
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        float testWidth = calculateWidth(testLine, scale);

        if (testWidth <= maxWidth) {
            currentLine = testLine;
        }
        else {
            // 当前行已满，添加到行列表
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
            }

            // 如果单个词就超过宽度，强制分割
            if (calculateWidth(word, scale) > maxWidth) {
                // 将长词分割为多个部分
                std::string part;
                for (char c : word) {
                    std::string testPart = part + std::string(1, c);
                    if (calculateWidth(testPart, scale) > maxWidth) {
                        lines.push_back(part);
                        part = std::string(1, c);
                    }
                    else {
                        part += c;
                    }
                }
                if (!part.empty()) {
                    currentLine = part;
                }
            }
            else {
                currentLine = word;
            }
        }
    }

    // 添加最后一行
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}

// 在指定矩形内渲染文本，自动换行并居中
void TextRenderer::RenderTextInBox(const std::string& text,
    float x, float y, float width, float height,
    float scale, glm::vec3 color) {
    // 分割文本为多行
    std::vector<std::string> lines = SplitTextIntoLines(text, width, scale);

    if (lines.empty()) return;

    // 计算文本块的总高度
    float textHeight = CalculateTextBlockHeight(lines, scale);

    // 计算起始Y位置（垂直居中）
    float startY = y + (height - textHeight) / 2.0f;

    // 使用'H'字符的高度作为行高参考
    float lineHeight = Characters['H'].Bearing.y * 1.5f * scale;

    // 渲染每一行
    for (size_t i = 0; i < lines.size(); i++) {
        const std::string& line = lines[i];

        // 计算行的宽度
        float lineWidth = calculateWidth(line, scale);

        // 计算起始X位置（水平居中）
        float startX = x + (width - lineWidth) / 2.0f;

        // 渲染这一行
        RenderText(line, startX, startY + i * lineHeight, scale, color);
    }
}