#pragma once

#include "plate.h"
#include "coordinate_trans.h"

class Plate3D : public Plate {
public:
	// 兼容旧构造函数：默认构造
	Plate3D();

	// 兼容旧构造函数签名，转发到 Plate 构造函数（保留默认值以兼容旧调用）
	Plate3D(glm::vec2 pos, float sizeX, float sizeY, int level, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	bool isChosen(float mouseX, float mouseY, Camera* cam = NULL) override;

	// 空方法
	virtual void DrawText(TextRenderer& textRenderer) override {};
	virtual void Draw(SpriteRenderer& renderer) override;
};