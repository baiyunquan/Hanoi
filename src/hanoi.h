#include <vector>
#include <glm/glm.hpp>

#include "game_object.h"
#include "plate.h"

class
	Hanoi {
	public:
		// 构造函数
		Hanoi(unsigned int numDisks , glm::vec2 size , bool empty);
		// 渲染汉诺塔
		void Draw(int posX , int posY);
		// 压入顶层盘子
		bool PushTop(Plate top);
		// 弹出顶层盘子
		Plate PopTop();

		bool isEmpty();
		bool isFull();
	
private:
	unsigned int numDisks;
	std::vector<Plate> disks;
	GameObject pole;
	GameObject base;

	glm::vec2 size;
}