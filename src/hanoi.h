#include <vector>
#include <glm/glm.hpp>

#include "game_object.h"
#include "plate.h"

class
	Hanoi {
	public:
		// ���캯��
		Hanoi(unsigned int numDisks , glm::vec2 size , bool empty);
		// ��Ⱦ��ŵ��
		void Draw(int posX , int posY);
		// ѹ�붥������
		bool PushTop(Plate top);
		// ������������
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