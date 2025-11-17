#include "object_3D.h"

void Object3D::Draw(SpriteRenderer& renderer) {
    // 使用 DrawCylinder2D 替代 DrawRectangle
    renderer.DrawCylinder2D(this->Position, glm::vec2(this->Width, this->Height), this->Rotation, this->Color);
}