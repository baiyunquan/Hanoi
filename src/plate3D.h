#include "plate.h"

class Plate3D : public Plate {
public:
    // 空方法
    virtual void DrawText(TextRenderer& textRenderer) override {};
    virtual bool isChosen(float mouseX, float mouseY) override { return false; };

    // Draw 方法使用 DrawCylinder2D
    virtual void Draw(SpriteRenderer& renderer) override;

    // 构造函数兼容老的构造函数
    Plate3D() : Plate() {}
    Plate3D(glm::vec2 pos, float sizeX, float sizeY, int level, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f))
        : Plate(pos, sizeX, sizeY, level, color, velocity) {}
};