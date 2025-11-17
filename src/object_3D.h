#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "object_2D.h"

// Container object for holding all state relevant for a single
// game object entity. Each object in the game likely needs the
// minimal of state as described within Object2D.
class Object3D : public Object2D
{
public:
	enum ObjectType {
		BOX , CYLINDER , CIRCLESLICE
	};

	// Getter for PosZ
	int getPosZ() const {
		return PosZ;
	}

	// Setter for PosZ
	void setPosZ(int z) {
		PosZ = z;
	}

	ObjectType getObjectType() {
		return objectType;
	}

	void setObjectType(ObjectType oj) {
		objectType = oj;
	}

	// Draw 方法使用 DrawCylinder2D
	void Draw(SpriteRenderer& renderer) override;

	// 空方法
	virtual void DrawText(TextRenderer& textRenderer) override {};
	virtual bool isChosen(float mouseX, float mouseY) override { return false; };

	// 构造函数兼容老的构造函数
	Object3D() : Object2D() {}
	Object3D(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f))
		: Object2D(pos, size, sprite, color, velocity) {}
	Object3D(glm::vec2 pos, float width, float height, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f))
		: Object2D(pos, width, height, color, velocity) {}

private :
	ObjectType objectType = BOX;
	int PosZ = 0;
};

#endif