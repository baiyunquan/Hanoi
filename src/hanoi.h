#include <map>
#include <glm/glm.hpp>

#include "game_object.h"

class
	Hanoi {
public:
	// ���캯��
	Hanoi(int numDisks, glm::vec2 pos, glm::vec2 size, bool empty);
	// ��Ⱦ��ŵ��
	void Draw(SpriteRenderer& renderer);
	// ѹ�붥������
	bool PushTop(GameObject top, int plateLevel);
	// ������������
	std::pair<int , GameObject> PopTop();

	bool isEmpty();
	bool isFull();

	int numDisks;
	std::map<int ,GameObject> disks;
	GameObject pole;
	GameObject base;

	glm::vec2 pos;
	glm::vec2 size;
	float unitHeight;
	float unitWidth;
};