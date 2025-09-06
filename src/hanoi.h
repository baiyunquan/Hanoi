#ifndef HANOI_H
#define HANOI_H

#include <map>
#include <glm/glm.hpp>

#include "game_object.h"
#include "plate.h"

class
	Hanoi {
public:
	// ���캯��
	Hanoi(int numDisks, glm::vec2 pos, glm::vec2 size, bool empty);
	// ��Ⱦ��ŵ��
	void Draw(SpriteRenderer& renderer, TextRenderer& textRenderer);
	// ѹ�붥������
	bool PushTop(Plate top, int plateLevel);
	// ������������
	std::pair<int , Plate> PopTop();

	bool isEmpty();
	bool isFull();
	int getTop();

	int numDisks;
	std::map<int , Plate> disks;
	GameObject pole;
	GameObject base;

	glm::vec2 pos;
	glm::vec2 size;
	float unitHeight;
	float unitWidth;
};

#endif