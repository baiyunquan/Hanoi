#include "plate3D.h"

// 默认构造，转发到 Plate::Plate()
Plate3D::Plate3D()
	: Plate()
{}

// 转发到 Plate(pos, sizeX, sizeY, level, color, velocity)
Plate3D::Plate3D(glm::vec2 pos, float sizeX, float sizeY, int level, glm::vec3 color, glm::vec2 velocity)
	: Plate(pos, sizeX, sizeY, level, color, velocity)
{}

bool Plate3D::isChosen(float mouseX, float mouseY, Camera* cam)
{
    if (cam == NULL) {
        return false;
    }

    CoordinateTrans* coordTrans = CoordinateTrans::getInstance();
    return coordTrans->cylinderIsChosen(
        cam->Position,
        cam->Front,
        this->Position,
        glm::vec2(this->Width, this->Height)
    );
}

void Plate3D::Draw(SpriteRenderer& renderer) {
    glm::vec3 drawColor = Color;
    if (isSelect()) drawColor = glm::vec3(1.0f, 0.0f, 0.0f);
	renderer.DrawCylinder2D(this->Position, glm::vec2(this->Width, this->Height), this->Rotation, drawColor);
}