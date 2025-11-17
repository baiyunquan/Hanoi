#include "plate.h"
#include <iostream>
#include <string>

Plate::Plate()
    : Object2D(), level(0){
}

// 构造函数的定义，初始化列表中调用基类构造函数
Plate::Plate(glm::vec2 pos, float sizeX, float sizeY, int level , glm::vec3 color, glm::vec2 velocity)
    : Object2D(pos, sizeX, sizeY, color, velocity) {
	this->level = level;
    std::cout << level << std::endl;
    this->setText(std::string("").append(std::to_string(level)));
}

void Plate::Draw(SpriteRenderer& renderer)
{
    if (selected) {
        renderer.DrawRectangle(this->Position, glm::vec2(this->Width , this->Height), this->Rotation, glm::vec3(1.0f, 0.0f, 0.0f));
        return;
    }
    renderer.DrawRectangle(this->Position, glm::vec2(this->Width , this->Height), this->Rotation, this->Color);
}