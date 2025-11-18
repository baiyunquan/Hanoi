#ifndef HANOI_H
#define HANOI_H

#include <map>
#include <glm/glm.hpp>

#include "object_3D.h"
#include "plate3D.h"

class
	Hanoi {
public:
	// 构造函数
	Hanoi(int numDisks, glm::vec2 pos, glm::vec2 size, bool empty);
	// 渲染汉诺塔
	void Draw(SpriteRenderer& renderer, TextRenderer& textRenderer);
	// 压入顶层盘子
	bool PushTop(Plate3D top, int plateLevel);
	// 弹出顶层盘子
	std::pair<int , Plate3D> PopTop();

	bool isEmpty();
	Plate3D* getTopPlate();
	bool isFull();
	int getTop();

	int numDisks;
	std::map<int , Plate3D> disks;
	Object3D pole;
	Object3D base;

	glm::vec2 pos;
	glm::vec2 size;
	float unitHeight;
	float unitWidth;
};

#endif