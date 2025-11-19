#ifndef OBJECT_2D_H
#define OBJECT_2D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

#include "texture.h"
#include "sprite_renderer.h"
#include "text_renderer.h"

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within Object2D.
class Object2D
{
public:
    // Object state
    glm::vec2   Position, Velocity;
    glm::vec3   Color;
    float     Rotation;
    bool      isRect;
    float Width, Height;

    // Render state
    Texture2D   Sprite;
    // Constructor(s)
    Object2D();
    Object2D(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    Object2D(glm::vec2 pos, float Width ,float  Height, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));
    // Draw sprite
    virtual void Draw(SpriteRenderer& renderer);
    // Draw text
    virtual void DrawText(TextRenderer& textRenderer);
    // 判断鼠标是否在Plate上
    virtual bool isChosen(float mouseX, float mouseY, Camera* cam = NULL);

    std::string getText();

    void setText(std::string str);
private :
    std::string text = "";
    float textScale = 1.5f;
};

#endif
