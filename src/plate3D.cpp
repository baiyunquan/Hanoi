#include "plate3D.h"

// 默认构造，转发到 Plate::Plate()
Plate3D::Plate3D()
	: Plate()
{}

// 转发到 Plate(pos, sizeX, sizeY, level, color, velocity)
Plate3D::Plate3D(glm::vec2 pos, float sizeX, float sizeY, int level, glm::vec3 color, glm::vec2 velocity)
	: Plate(pos, sizeX, sizeY, level, color, velocity)
{}

void Plate3D::Draw(SpriteRenderer& renderer) {
	renderer.DrawCylinder2D(this->Position, glm::vec2(this->Width, this->Height), this->Rotation, this->Color);
}