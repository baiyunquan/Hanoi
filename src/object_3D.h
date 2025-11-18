#pragma once

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

	// 兼容旧构造函数：默认构造
	Object3D();

	// 兼容旧构造函数签名（转发到 Object2D），末尾附加可选 PosZ 与 ObjectType（带默认值，向后兼容）
	Object3D(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f), int posZ = 0, ObjectType type = BOX);
	Object3D(glm::vec2 pos, float Width , float Height, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f), int posZ = 0, ObjectType type = BOX);

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

	void Draw(SpriteRenderer& renderer);
	// 空方法
	virtual void DrawText(TextRenderer& textRenderer) {};
	virtual bool isChosen(float mouseX, float mouseY) { return false; };
private :
	ObjectType objectType = BOX;
	int PosZ = 0;
};