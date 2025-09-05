#include <map>
#include <glm/glm.hpp>

#include "game_object.h"

class
	Hanoi {
public:
	// 构造函数
	Hanoi(int numDisks, glm::vec2 pos, glm::vec2 size, bool empty);
	// 渲染汉诺塔
	void Draw(SpriteRenderer& renderer);
	// 压入顶层盘子
	bool PushTop(GameObject top, int plateLevel);
	// 弹出顶层盘子
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