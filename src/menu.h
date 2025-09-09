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
    Button* okButton; // 添加OK按钮

    Menu(float width, float height);
    void Draw(SpriteRenderer& sr, TextRenderer& tr, float width, float height);
    ~Menu(); // 添加析构函数

    void Update(float dt); // 添加更新函数
    void mouseClick(float x, float y);
    void SetCallback(std::function<void(int, int, bool, float)> callback); // 设置回调函数

private:
    std::function<void(int, int, bool, float)> callBack;

    // 菜单状态
    int towerCount = 3;
    int diskCount = 5;
    bool soundEnabled = true;
    float volume = 1.0f;

    // 常量定义
    const int MIN_TOWERS = 3;
    const int MAX_TOWERS = 8;
    const int MIN_DISKS = 3;
    const int MAX_DISKS = 10;
};