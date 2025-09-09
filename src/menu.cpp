#include "menu.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Menu::Menu(float width, float height)
{
    // 初始化按钮
    float buttonWidth = width * 0.15f;
    float buttonHeight = height * 0.05f;
    float buttonSpacing = height * 0.02f;

    // 计算按钮位置
    float leftColumnX = width * 0.2f;
    float rightColumnX = width * 0.6f;
    float startY = height * 0.35f;

    // 塔数量控制按钮
    towerAdd = new Button(glm::vec2(leftColumnX, startY), buttonWidth, buttonHeight, glm::vec3(1.0f));
    towerAdd->setText("+");

    towerSub = new Button(glm::vec2(rightColumnX, startY), buttonWidth, buttonHeight, glm::vec3(1.0f));
    towerSub->setText("-");

    // 盘子数量控制按钮
    diskAdd = new Button(glm::vec2(leftColumnX, startY + buttonHeight + buttonSpacing),
        buttonWidth, buttonHeight, glm::vec3(0.2f, 0.6f, 0.3f));
    diskAdd->setText("+");

    diskSub = new Button(glm::vec2(rightColumnX, startY + buttonHeight + buttonSpacing),
        buttonWidth, buttonHeight, glm::vec3(0.8f, 0.2f, 0.2f));
    diskSub->setText("-");

    // 声音开关按钮
    soundOn = new Button(glm::vec2(leftColumnX, startY + 2 * (buttonHeight + buttonSpacing)),
        buttonWidth * 2 + (rightColumnX - leftColumnX - buttonWidth), buttonHeight,
        glm::vec3(0.3f, 0.4f, 0.8f));
    soundOn->setText("ON");

    // 音量控制按钮
    volumeAdd = new Button(glm::vec2(leftColumnX, startY + 3 * (buttonHeight + buttonSpacing)),
        buttonWidth, buttonHeight, glm::vec3(0.2f, 0.6f, 0.3f));
    volumeAdd->setText("+");

    volumeSub = new Button(glm::vec2(rightColumnX, startY + 3 * (buttonHeight + buttonSpacing)),
        buttonWidth, buttonHeight, glm::vec3(0.8f, 0.2f, 0.2f));
    volumeSub->setText("-");

    // OK按钮
    float okButtonY = startY + 4 * (buttonHeight + buttonSpacing);
    okButton = new Button(glm::vec2(width * 0.4f, okButtonY),
        width * 0.2f, buttonHeight * 1.5f, glm::vec3(0.1f, 0.7f, 0.1f));
    okButton->setText("OK");
}

void Menu::Draw(SpriteRenderer sr, TextRenderer tr, float width, float height)
{
    // 绘制所有按钮
    towerAdd->Draw(sr);

    towerSub->Draw(sr);


    diskAdd->Draw(sr);


    diskSub->Draw(sr);


    soundOn->Draw(sr);


    volumeAdd->Draw(sr);


    volumeSub->Draw(sr);

    okButton->Draw(sr);


    // 绘制标题 - 占用上方30%空间
    float titleHeight = height * 0.3f;
    tr.RenderTextInBox("Hanoi Tower", 0, 0, width, titleHeight, 2.0f, glm::vec3(0.8f, 1.0f, 0.7f));

    // 绘制设置区域 - 占用下方70%空间
    float settingsY = titleHeight;
    float settingsHeight = height - titleHeight;

    // 绘制设置项标签
    float labelX = width * 0.05f;
    float valueX = width * 0.35f;
    float startY = settingsY + settingsHeight * 0.1f;
    float rowHeight = settingsHeight * 0.15f;

    // 塔数量
    tr.RenderTextInBox("Towers:", labelX, startY, width * 0.3f, rowHeight, 1.2f, glm::vec3(1.0f));
    tr.RenderTextInBox(std::to_string(towerCount), valueX, startY, width * 0.2f, rowHeight, 1.2f, glm::vec3(1.0f));

    // 盘子数量
    tr.RenderTextInBox("Disks:", labelX, startY + rowHeight, width * 0.3f, rowHeight, 1.2f, glm::vec3(1.0f));
    tr.RenderTextInBox(std::to_string(diskCount), valueX, startY + rowHeight, width * 0.2f, rowHeight, 1.2f, glm::vec3(1.0f));

    // 声音开关
    tr.RenderTextInBox("Sound:", labelX, startY + 2 * rowHeight, width * 0.3f, rowHeight, 1.2f, glm::vec3(1.0f));
//    tr.RenderTextInBox(soundEnabled ? "ON" : "OFF", valueX, startY + 2 * rowHeight, width * 0.2f, rowHeight, 1.2f,
        //soundEnabled ? glm::vec3(0.2f, 0.6f, 0.2f) : glm::vec3(0.8f, 0.2f, 0.2f));

    // 音量
    tr.RenderTextInBox("Volume:", labelX, startY + 3 * rowHeight, width * 0.3f, rowHeight, 1.2f, glm::vec3(1.0f));
    std::stringstream volumeStr;
    volumeStr << std::fixed << std::setprecision(1) << volume;
    tr.RenderTextInBox(volumeStr.str(), valueX, startY + 3 * rowHeight, width * 0.2f, rowHeight, 1.2f, glm::vec3(1.0f));
}

void Menu::mouseClick(float x, float y)
{
    // 检查每个按钮是否被点击
    if (towerAdd->isChosen(x, y)) {
        towerAdd->StartBounceAnimation();
        towerCount = std::min(MAX_TOWERS, towerCount + 1);
    }
    else if (towerSub->isChosen(x, y)) {
        towerSub->StartBounceAnimation();
        towerCount = std::max(MIN_TOWERS, towerCount - 1);
    }
    else if (diskAdd->isChosen(x, y)) {
        diskAdd->StartBounceAnimation();
        diskCount = std::min(MAX_DISKS, diskCount + 1);
    }
    else if (diskSub->isChosen(x, y)) {
        diskSub->StartBounceAnimation();
        diskCount = std::max(MIN_DISKS, diskCount - 1);
    }
    else if (soundOn->isChosen(x, y)) {
        soundOn->StartBounceAnimation();
        soundEnabled = !soundEnabled;
        soundOn->setText(soundEnabled ? "ON" : "OFF");
    }
    else if (volumeAdd->isChosen(x, y)) {
        volumeAdd->StartBounceAnimation();
        volume = std::min(1.0f, volume + 0.1f);
    }
    else if (volumeSub->isChosen(x, y)) {
        volumeSub->StartBounceAnimation();
        volume = std::max(0.0f, volume - 0.1f);
    }
    else if (okButton->isChosen(x, y)) {
        okButton->StartBounceAnimation();
        if (callBack) {
            callBack(towerCount, diskCount, soundEnabled, volume);
        }
    }
}

// 添加更新函数来处理动画
void Menu::Update(float dt)
{
    towerAdd->UpdateBounceAnimation(dt);
    towerSub->UpdateBounceAnimation(dt);
    diskAdd->UpdateBounceAnimation(dt);
    diskSub->UpdateBounceAnimation(dt);
    soundOn->UpdateBounceAnimation(dt);
    volumeAdd->UpdateBounceAnimation(dt);
    volumeSub->UpdateBounceAnimation(dt);
    okButton->UpdateBounceAnimation(dt);
}

// 设置回调函数
void Menu::SetCallback(std::function<void(int, int, bool, float)> callback)
{
    this->callBack = callback;
}