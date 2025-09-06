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

    // �����ı�������ַ�����͵�bearing.y
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

    // �����ַ�������������Ĳ��֣���'g', 'p', 'q'�ȣ�
    // �ܸ߶� = ����ַ��ĸ߶� + ��������Ĳ���
    return maxHeight + (Characters['H'].Bearing.y - minBearingY) * scale;
}

// �����ı���ָ����Ⱥ͸߶��µ�������ű���
float TextRenderer::CalculateOptimalScale(const std::string& text,
    float availableWidth,
    float availableHeight) {
    if (text.empty() || availableWidth <= 0 || availableHeight <= 0) {
        return 1.0f;
    }

    // ��ʼ���ű���
    float scale = 1.0f;
    float minScale = 0.1f;
    float maxScale = 10.0f;

    // ʹ�ö��ֲ��ҷ��ҵ�������ű���
    for (int i = 0; i < 10; i++) { // ����10��Ӧ���㹻��ȷ
        // ʹ�õ�ǰ���ű����ָ��ı�
        std::vector<std::string> lines = SplitTextIntoLines(text, availableWidth, scale);

        // �����ı���߶�
        float textHeight = CalculateTextBlockHeight(lines, scale);

        // ����Ƿ��ʺϿ��ø߶�
        if (textHeight <= availableHeight) {
            // �ı��߶��ʺϣ������������ű���
            minScale = scale;
            scale = (scale + maxScale) / 2.0f;
        }
        else {
            // �ı��߶�̫�󣬼�С���ű���
            maxScale = scale;
            scale = (minScale + scale) / 2.0f;
        }
    }

    return scale;
}

// �Զ��������ű���������������Ⱦ�ı�
void TextRenderer::RenderTextAutoScale(const std::string& text,
    float x, float y, float width, float height,
    glm::vec3 color) {
    if (text.empty() || width <= 0 || height <= 0) {
        return;
    }

    // ����������ű���
    float optimalScale = 1.0;
    if (text.size() > 10) {
        optimalScale = CalculateOptimalScale(text, width, height);
    }

    // ʹ��������ű�����Ⱦ�ı�
    RenderTextInBox(text, x, y, width, height, optimalScale, color);
}

// �Ľ����ı���߶ȼ���
float TextRenderer::CalculateTextBlockHeight(const std::vector<std::string>& lines, float scale) {
    if (lines.empty()) return 0.0f;

    // ʹ�������ʵ�ʶ�����Ϣ�����и�
    // ���û�б������������Ϣ��ʹ���ַ�'H'�ĸ߶���Ϊ�ο�
    float lineHeight;
    if (Characters.find('H') != Characters.end()) {
        // �и� = �ַ��߶� + һЩ������
        lineHeight = (Characters['H'].Size.y + Characters['H'].Bearing.y * 0.2f) * scale;
    }
    else if (!Characters.empty()) {
        // ʹ�õ�һ�������ַ�
        Character firstChar = Characters.begin()->second;
        lineHeight = (firstChar.Size.y + firstChar.Bearing.y * 0.2f) * scale;
    }
    else {
        // Ĭ���и�
        lineHeight = 20.0f * scale;
    }

    return lines.size() * lineHeight;
}

// �Ľ����ı��ָ��㷨
std::vector<std::string> TextRenderer::SplitTextIntoLines(const std::string& text, float maxWidth, float scale) {
    std::vector<std::string> lines;

    // �������Ҫ���У�����㹻����ֱ�ӷ��ص���
    if (calculateWidth(text, scale) <= maxWidth) {
        lines.push_back(text);
        return lines;
    }

    std::istringstream stream(text);
    std::string word;
    std::string currentLine;

    while (stream >> word) {
        // �����������ʺ��Ƿ�ᳬ�����
        std::string testLine = currentLine.empty() ? word : currentLine + " " + word;
        float testWidth = calculateWidth(testLine, scale);

        if (testWidth <= maxWidth) {
            currentLine = testLine;
        }
        else {
            // ��ǰ����������ӵ����б�
            if (!currentLine.empty()) {
                lines.push_back(currentLine);
            }

            // ��������ʾͳ�����ȣ�ǿ�Ʒָ�
            if (calculateWidth(word, scale) > maxWidth) {
                // �����ʷָ�Ϊ�������
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

    // ������һ��
    if (!currentLine.empty()) {
        lines.push_back(currentLine);
    }

    return lines;
}

// ��ָ����������Ⱦ�ı����Զ����в�����
void TextRenderer::RenderTextInBox(const std::string& text,
    float x, float y, float width, float height,
    float scale, glm::vec3 color) {
    // �ָ��ı�Ϊ����
    std::vector<std::string> lines = SplitTextIntoLines(text, width, scale);

    if (lines.empty()) return;

    // �����ı�����ܸ߶�
    float textHeight = CalculateTextBlockHeight(lines, scale);

    // ������ʼYλ�ã���ֱ���У�
    float startY = y + (height - textHeight) / 2.0f;

    // ʹ��'H'�ַ��ĸ߶���Ϊ�и߲ο�
    float lineHeight = Characters['H'].Bearing.y * 1.5f * scale;

    // ��Ⱦÿһ��
    for (size_t i = 0; i < lines.size(); i++) {
        const std::string& line = lines[i];

        // �����еĿ��
        float lineWidth = calculateWidth(line, scale);

        // ������ʼXλ�ã�ˮƽ���У�
        float startX = x + (width - lineWidth) / 2.0f;

        // ��Ⱦ��һ��
        RenderText(line, startX, startY + i * lineHeight, scale, color);
    }
}