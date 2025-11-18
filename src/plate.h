#ifndef PLATE_H
#define PLATE_H

#include "object_2D.h"

class Plate : public Object2D {
public:
    Plate();
    Plate(glm::vec2 pos, float sizeX, float sizeY,int level , glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

	int level; // 盘子的大小等级，数字越小表示盘子越大

    void select() { selected = !selected; }

    bool isSelect() { return selected; }
    // Draw sprite
    void Draw(SpriteRenderer& renderer);
private:
    bool selected = false; // 盘子是否被选中
};

#endif