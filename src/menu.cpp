#include "menu.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Menu::Menu(float width, float height)
{
    // 设置区域 - 占屏幕底部70%区域
    float centerX = width * 0.5f;
    float settingsY = height * 0.3f;
    float settingsHeight = height * 0.7f;

    float buttonColumnX = width * 0.65f;
    float buttonWidth = width * 0.12f;
    float buttonHeight = height * 0.07f;
    float buttonSpacing = height * 0.03f;

    // 计算列位置和行高 - 确保对齐
    float startY = settingsY + settingsHeight * 0.1f + 10.0f;
    float rowHeight = settingsHeight * 0.15f;

    // 塔数量加减按钮 - 确保大小和位置完全一致
    towerAdd = new Button(glm::vec2(buttonColumnX, startY), 
                         buttonWidth, buttonHeight, glm::vec3(0.2f, 0.6f, 0.3f));
    towerAdd->setText("+");

    towerSub = new Button(glm::vec2(buttonColumnX + buttonWidth + buttonSpacing * 0.5f, startY), 
                         buttonWidth, buttonHeight, glm::vec3(0.8f, 0.2f, 0.2f));
    towerSub->setText("-");

    // 盘数量加减按钮 - 确保大小和位置完全一致
    diskAdd = new Button(glm::vec2(buttonColumnX, startY + rowHeight),
                        buttonWidth, buttonHeight, glm::vec3(0.2f, 0.6f, 0.3f));
    diskAdd->setText("+");

    diskSub = new Button(glm::vec2(buttonColumnX + buttonWidth + buttonSpacing * 0.5f, startY + rowHeight),
                        buttonWidth, buttonHeight, glm::vec3(0.8f, 0.2f, 0.2f));
    diskSub->setText("-");

    // 声音开关按钮
    soundOn = new Button(glm::vec2(buttonColumnX, startY + 2 * rowHeight),
                        buttonWidth * 2 + buttonSpacing * 0.5f, buttonHeight, glm::vec3(0.3f, 0.4f, 0.8f));

    // 音量加减按钮 - 确保大小和位置完全一致
    volumeAdd = new Button(glm::vec2(buttonColumnX, startY + 3 * rowHeight),
                          buttonWidth, buttonHeight, glm::vec3(0.2f, 0.6f, 0.3f));
    volumeAdd->setText("+");

    volumeSub = new Button(glm::vec2(buttonColumnX + buttonWidth + buttonSpacing * 0.5f, startY + 3 * rowHeight),
                          buttonWidth, buttonHeight, glm::vec3(0.8f, 0.2f, 0.2f));
    volumeSub->setText("-");

    // OK按钮
    okButton = new Button(glm::vec2(centerX - width * 0.125f, startY + 4.5f * rowHeight),
                         width * 0.25f, buttonHeight * 1.3f, glm::vec3(0.1f, 0.7f, 0.1f));
    okButton->setText("OK");
}

void Menu::Draw(SpriteRenderer& sr, TextRenderer& tr, float width, float height)
{
    // 绘制标题 - 占屏幕顶部30%区域
    float titleHeight = height * 0.3f;
    tr.RenderTextInBox("Hanoi Tower", 0, 0, width, titleHeight, 2.0f, glm::vec3(0.4f, 0.4f, 0.8f));

    // 设置区域 - 占屏幕底部70%区域
    float settingsY = titleHeight;
    float settingsHeight = height - titleHeight;

    // 计算列位置和行高 - 确保对齐
    float labelColumnX = width * 0.25f;
    float valueColumnX = width * 0.45f;
    float columnWidth = width * 0.15f;
    float startY = settingsY + settingsHeight * 0.1f;
    float rowHeight = settingsHeight * 0.15f;

    // 塔数量 - 确保标签、数值和按钮对齐
    tr.RenderTextInBox("Towers:", labelColumnX, startY, columnWidth, rowHeight, 1.2f, glm::vec3(1.0f));
    tr.RenderTextInBox(std::to_string(towerCount), valueColumnX, startY, columnWidth, rowHeight, 1.2f, glm::vec3(1.0f));

    // 盘数量 - 确保标签、数值和按钮对齐
    tr.RenderTextInBox("Disks:", labelColumnX, startY + rowHeight, columnWidth, rowHeight, 1.2f, glm::vec3(1.0f));
    tr.RenderTextInBox(std::to_string(diskCount), valueColumnX, startY + rowHeight, columnWidth, rowHeight, 1.2f, glm::vec3(1.0f));

    // 声音设置 - 确保标签、数值和按钮对齐
    tr.RenderTextInBox("Sound:", labelColumnX, startY + 2 * rowHeight, columnWidth, rowHeight, 1.2f, glm::vec3(1.0f));
    tr.RenderTextInBox(soundEnabled ? "ON" : "OFF", valueColumnX, startY + 2 * rowHeight, columnWidth, rowHeight, 1.2f,
                      soundEnabled ? glm::vec3(0.2f, 0.6f, 0.2f) : glm::vec3(0.8f, 0.2f, 0.2f));

    // 音量设置 - 确保标签、数值和按钮对齐
    tr.RenderTextInBox("Volume:", labelColumnX, startY + 3 * rowHeight, columnWidth, rowHeight, 1.2f, glm::vec3(1.0f));
    std::stringstream volumeStr;
    volumeStr << std::fixed << std::setprecision(1) << volume;
    tr.RenderTextInBox(volumeStr.str(), valueColumnX, startY + 3 * rowHeight, columnWidth, rowHeight, 1.2f, glm::vec3(1.0f));

    // 绘制所有按钮
    towerAdd->Draw(sr);
    towerAdd->DrawText(tr);

    towerSub->Draw(sr);
    towerSub->DrawText(tr);

    diskAdd->Draw(sr);
    diskAdd->DrawText(tr);

    diskSub->Draw(sr);
    diskSub->DrawText(tr);

    soundOn->Draw(sr);
    soundOn->setText(soundEnabled ? "ON" : "OFF");
    soundOn->DrawText(tr);

    volumeAdd->Draw(sr);
    volumeAdd->DrawText(tr);

    volumeSub->Draw(sr);
    volumeSub->DrawText(tr);

    okButton->Draw(sr);
    okButton->DrawText(tr);
}

Menu::~Menu()
{
    delete towerAdd;
    delete towerSub;
    delete diskAdd;
    delete diskSub;
    delete soundOn;
    delete volumeAdd;
    delete volumeSub;
    delete okButton;
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

// 更新按钮动画状态
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