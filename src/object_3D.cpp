#include "object_3D.h"

// 默认构造，转发到 Object2D 的默认构造
Object3D::Object3D()
	: Object2D(), objectType(BOX), PosZ(0)
{}

// 转发到 Object2D(pos, size, sprite, color, velocity)
Object3D::Object3D(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity, int posZ, ObjectType type)
	: Object2D(pos, size, sprite, color, velocity), objectType(type), PosZ(posZ)
{}

// 转发到 Object2D(pos, Width, Height, color, velocity)
Object3D::Object3D(glm::vec2 pos, float Width, float Height, glm::vec3 color, glm::vec2 velocity, int posZ, ObjectType type)
	: Object2D(pos, Width, Height, color, velocity), objectType(type), PosZ(posZ)
{}

void Object3D::Draw(SpriteRenderer& renderer) {
	renderer.DrawCylinder2D(this->Position, glm::vec2(this->Width, this->Height), this->Rotation, this->Color);
}