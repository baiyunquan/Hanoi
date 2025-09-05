#ifndef PLATE_H
#define PLATE_H

#include "game_object.h"

class Plate : public GameObject {
public:

	// 默认构造函数
    Plate();
    // 声明构造函数，使用默认参数，注意：默认参数只能出现在声明中
    Plate(glm::vec2 pos, float sizeX, float sizeY,int level , glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    // 判断鼠标是否在Plate上
    bool isChosen(int mouseX, int mouseY);

	int level; // 盘子的大小等级，数字越小表示盘子越大

    void select() { selected = !selected; }
    // Draw sprite
    void Draw(SpriteRenderer& renderer);
private:
    bool selected = false; // 盘子是否被选中
};

#endif