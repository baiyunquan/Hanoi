#pragma once

#include "game_object.h"

class Button : public GameObject {
public:
    // ʹ�ü̳й��캯�� - �Զ��̳� GameObject �����й��캯��
    using GameObject::GameObject;

    // ��������״̬
    enum BounceState {
        NONE,
        SHRINKING,
        EXPANDING
    };

    // ������������
    void StartBounceAnimation(float duration = 0.3f); // ���̶���ʱ��
    void UpdateBounceAnimation(float dt);
    bool IsAnimating() const { return bounceState != NONE; }

    // ��д Draw ����
    void Draw(SpriteRenderer& renderer) override;

private:
    // ������������
    BounceState bounceState = NONE;
    float bounceProgress = 0.0f;
    float bounceDuration = 0.3f; // ���̶���ʱ��
    float originalSizeX, originalSizeY;
};