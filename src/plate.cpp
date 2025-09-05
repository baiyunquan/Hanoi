#include "plate.h"
#include <iostream>

Plate::Plate()
    : GameObject() {
}

// ���캯���Ķ��壬��ʼ���б��е��û��๹�캯��
Plate::Plate(glm::vec2 pos, float sizeX, float sizeY, int level , glm::vec3 color, glm::vec2 velocity)
    : GameObject(pos, sizeX, sizeY, color, velocity) {
	this->level = level;
    std::cout << level << std::endl;
}

bool Plate::isChosen(int mouseX, int mouseY) {
    return mouseX >= Position.x && mouseX <= Position.x + SizeX &&
        mouseY >= Position.y && mouseY <= Position.y + SizeY;
}

void Plate::Draw(SpriteRenderer& renderer)
{
    if (selected) {
        renderer.DrawRectangle(this->Position, glm::vec2(this->SizeX , this->SizeY), this->Rotation, glm::vec3(1.0f, 0.0f, 0.0f));
        return;
    }
    renderer.DrawRectangle(this->Position, glm::vec2(this->SizeX , this->SizeY), this->Rotation, this->Color);
}