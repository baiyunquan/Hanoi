#ifndef PLATE_H
#define PLATE_H

#include "game_object.h"

class Plate : public GameObject {
public:

	// Ĭ�Ϲ��캯��
    Plate();
    // �������캯����ʹ��Ĭ�ϲ�����ע�⣺Ĭ�ϲ���ֻ�ܳ�����������
    Plate(glm::vec2 pos, float sizeX, float sizeY,int level , glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    // �ж�����Ƿ���Plate��
    bool isChosen(int mouseX, int mouseY);

	int level; // ���ӵĴ�С�ȼ�������ԽС��ʾ����Խ��

    void select() { selected = !selected; }
    // Draw sprite
    void Draw(SpriteRenderer& renderer);
private:
    bool selected = false; // �����Ƿ�ѡ��
};

#endif