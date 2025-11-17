#include "plate3D.h"

void Plate3D::Draw(SpriteRenderer& renderer) {
	renderer.DrawCylinder2D(this->Position, glm::vec2(this->Width, this->Height), this->Rotation, this->Color);
}