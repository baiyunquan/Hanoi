#pragma once

#include <functional>

#include "button.h"
#include "text_renderer.h"
#include "sprite_renderer.h"

class Menu {
public:
    Button* towerAdd, * towerSub;
    Button* diskAdd, * diskSub;
    Button* soundOn;
    Button* volumeAdd, * volumeSub;
    Button* okButton; // ���OK��ť

    Menu(float width, float height);
    void Draw(SpriteRenderer& sr, TextRenderer& tr, float width, float height);
    ~Menu(); // �����������

    void Update(float dt); // ��Ӹ��º���
    void mouseClick(float x, float y);
    void SetCallback(std::function<void(int, int, bool, float)> callback); // ���ûص�����

private:
    std::function<void(int, int, bool, float)> callBack;

    // �˵�״̬
    int towerCount = 3;
    int diskCount = 5;
    bool soundEnabled = true;
    float volume = 1.0f;

    // ��������
    const int MIN_TOWERS = 3;
    const int MAX_TOWERS = 8;
    const int MIN_DISKS = 3;
    const int MAX_DISKS = 10;
};