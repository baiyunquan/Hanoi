#include "menu.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Menu::Menu(float width, float height)
{
    // ��ʼ����ť
    float buttonWidth = width * 0.15f;
    float buttonHeight = height * 0.05f;
    float buttonSpacing = height * 0.02f;

    // ���㰴ťλ��
    float leftColumnX = width * 0.2f;
    float rightColumnX = width * 0.6f;
    float startY = height * 0.35f;

    // ���������ư�ť
    towerAdd = new Button(glm::vec2(leftColumnX, startY), buttonWidth, buttonHeight, glm::vec3(1.0f));
    towerAdd->setText("+");

    towerSub = new Button(glm::vec2(rightColumnX, startY), buttonWidth, buttonHeight, glm::vec3(1.0f));
    towerSub->setText("-");

    // �����������ư�ť
    diskAdd = new Button(glm::vec2(leftColumnX, startY + buttonHeight + buttonSpacing),
        buttonWidth, buttonHeight, glm::vec3(0.2f, 0.6f, 0.3f));
    diskAdd->setText("+");

    diskSub = new Button(glm::vec2(rightColumnX, startY + buttonHeight + buttonSpacing),
        buttonWidth, buttonHeight, glm::vec3(0.8f, 0.2f, 0.2f));
    diskSub->setText("-");

    // �������ذ�ť
    soundOn = new Button(glm::vec2(leftColumnX, startY + 2 * (buttonHeight + buttonSpacing)),
        buttonWidth * 2 + (rightColumnX - leftColumnX - buttonWidth), buttonHeight,
        glm::vec3(0.3f, 0.4f, 0.8f));
    soundOn->setText("ON");

    // �������ư�ť
    volumeAdd = new Button(glm::vec2(leftColumnX, startY + 3 * (buttonHeight + buttonSpacing)),
        buttonWidth, buttonHeight, glm::vec3(0.2f, 0.6f, 0.3f));
    volumeAdd->setText("+");

    volumeSub = new Button(glm::vec2(rightColumnX, startY + 3 * (buttonHeight + buttonSpacing)),
        buttonWidth, buttonHeight, glm::vec3(0.8f, 0.2f, 0.2f));
    volumeSub->setText("-");

    // OK��ť
    float okButtonY = startY + 4 * (buttonHeight + buttonSpacing);
    okButton = new Button(glm::vec2(width * 0.4f, okButtonY),
        width * 0.2f, buttonHeight * 1.5f, glm::vec3(0.1f, 0.7f, 0.1f));
    okButton->setText("OK");
}

void Menu::Draw(SpriteRenderer sr, TextRenderer tr, float width, float height)
{
    // �������а�ť
    towerAdd->Draw(sr);

    towerSub->Draw(sr);


    diskAdd->Draw(sr);


    diskSub->Draw(sr);


    soundOn->Draw(sr);


    volumeAdd->Draw(sr);


    volumeSub->Draw(sr);

    okButton->Draw(sr);


    // ���Ʊ��� - ռ���Ϸ�30%�ռ�
    float titleHeight = height * 0.3f;
    tr.RenderTextInBox("Hanoi Tower", 0, 0, width, titleHeight, 2.0f, glm::vec3(0.8f, 1.0f, 0.7f));

    // ������������ - ռ���·�70%�ռ�
    float settingsY = titleHeight;
    float settingsHeight = height - titleHeight;

    // �����������ǩ
    float labelX = width * 0.05f;
    float valueX = width * 0.35f;
    float startY = settingsY + settingsHeight * 0.1f;
    float rowHeight = settingsHeight * 0.15f;

    // ������
    tr.RenderTextInBox("Towers:", labelX, startY, width * 0.3f, rowHeight, 1.2f, glm::vec3(1.0f));
    tr.RenderTextInBox(std::to_string(towerCount), valueX, startY, width * 0.2f, rowHeight, 1.2f, glm::vec3(1.0f));

    // ��������
    tr.RenderTextInBox("Disks:", labelX, startY + rowHeight, width * 0.3f, rowHeight, 1.2f, glm::vec3(1.0f));
    tr.RenderTextInBox(std::to_string(diskCount), valueX, startY + rowHeight, width * 0.2f, rowHeight, 1.2f, glm::vec3(1.0f));

    // ��������
    tr.RenderTextInBox("Sound:", labelX, startY + 2 * rowHeight, width * 0.3f, rowHeight, 1.2f, glm::vec3(1.0f));
//    tr.RenderTextInBox(soundEnabled ? "ON" : "OFF", valueX, startY + 2 * rowHeight, width * 0.2f, rowHeight, 1.2f,
        //soundEnabled ? glm::vec3(0.2f, 0.6f, 0.2f) : glm::vec3(0.8f, 0.2f, 0.2f));

    // ����
    tr.RenderTextInBox("Volume:", labelX, startY + 3 * rowHeight, width * 0.3f, rowHeight, 1.2f, glm::vec3(1.0f));
    std::stringstream volumeStr;
    volumeStr << std::fixed << std::setprecision(1) << volume;
    tr.RenderTextInBox(volumeStr.str(), valueX, startY + 3 * rowHeight, width * 0.2f, rowHeight, 1.2f, glm::vec3(1.0f));
}

void Menu::mouseClick(float x, float y)
{
    // ���ÿ����ť�Ƿ񱻵��
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

// ��Ӹ��º�����������
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

// ���ûص�����
void Menu::SetCallback(std::function<void(int, int, bool, float)> callback)
{
    this->callBack = callback;
}