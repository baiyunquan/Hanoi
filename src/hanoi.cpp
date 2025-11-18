#include <iostream>

#include "hanoi.h"

Hanoi::Hanoi(int numDisks, glm::vec2 pos, glm::vec2 size, bool empty)
	:numDisks(numDisks) , size(size) , pos(pos)
{
	unitHeight = size.y / (float)(numDisks + 1);
	unitWidth = size.x / (float)(numDisks + 3);

	pole = Object3D(glm::vec2(pos.x + (size.x / 2) - (unitWidth / 8), pos.y), unitWidth / 4,
		unitHeight / 2, glm::vec3(1.0, 1.0f, 1.0f));
	base = Object3D(glm::vec2(pos.x, pos.y + size.y - (unitHeight / 2)), size.x,
		unitHeight / 2, glm::vec3(1.0, 1.0f, 1.0f));

	if (!empty) {
		float length = unitWidth;
		float middle = pos.x + (size.x / 2);
		float bottom = pos.y + (unitHeight / 2);
		for(int i = 1; i <= numDisks; i++) {
			std::cout << "Creating disk level " << i << " at position (" 
				<< (middle - (length / 2)) << ", " << bottom << ") with size (" << length << ", " << unitHeight << ")\n";

			disks[i] = Plate3D(glm::vec2(middle - (length / 2), bottom),
				length, unitHeight, i ,
				glm::vec3(0.5f + 0.1f * (i / (float)numDisks), 0.5f, 1.0f - 0.5f * (i / (float)numDisks)));
			length += unitWidth;
			bottom += unitHeight;
		}
	}
	else {
		pole.Height += (float)(numDisks)*unitHeight;
	}
}

void Hanoi::Draw(SpriteRenderer& renderer , TextRenderer& textRenderer) {
	pole.Height = (unitHeight / 2) + ((float)(numDisks - disks.size()) * unitHeight);
//	std::cout << pole.Height << std::endl;

	base.Draw(renderer);
	base.DrawText(textRenderer);
	pole.Draw(renderer);

	float bottom = pole.Position.y + pole.Height;
	for(auto& [level , disk] : disks) {
		/*std::cout << "Drawing disk level " << level << " at position (" 
			<< disk.Position.x << ", " << bottom << ") with size (" << disk.Width << ", " << disk.Height << ")\n";*/
		disk.Position.y = bottom;
		disk.Draw(renderer);
		disk.DrawText(textRenderer);
		bottom += unitHeight;
	}
}

bool Hanoi::PushTop(Plate3D top , int plateLevel) {
	if (isFull() || plateLevel > getTop()) return false;

	disks[plateLevel] = top;
	return true;
}

int Hanoi::getTop() {
	if (!isEmpty()) {
		return disks.begin()->first;
	}
	return numDisks + 1;
}

std::pair<int , Plate3D> Hanoi::PopTop() {
	if (isEmpty()) return { -1 , Plate3D()};
	auto it = disks.begin();
	std::pair<int , Plate3D> res = *it;
	disks.erase(it->first);
	return res;
}

bool Hanoi::isEmpty() {
	return disks.empty();
}

Plate3D* Hanoi::getTopPlate() {
	if (!isEmpty()) {
		return &disks.begin()->second;
	}
	return nullptr;
}

bool Hanoi::isFull() {
	return disks.size() >= (unsigned int)numDisks;
}