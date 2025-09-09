#pragma once

#include "game_object.h"

class Button : public GameObject {
public:
    // 使用继承构造函数 - 自动继承 GameObject 的所有构造函数
    using GameObject::GameObject;

    // 反弹动画状态
    enum BounceState {
        NONE,
        SHRINKING,
        EXPANDING
    };

    // 反弹动画方法
    void StartBounceAnimation(float duration = 0.3f); // 缩短动画时间
    void UpdateBounceAnimation(float dt);
    bool IsAnimating() const { return bounceState != NONE; }

    // 重写 Draw 方法
    void Draw(SpriteRenderer& renderer) override;

private:
    // 反弹动画属性
    BounceState bounceState = NONE;
    float bounceProgress = 0.0f;
    float bounceDuration = 0.3f; // 缩短动画时间
    float originalSizeX, originalSizeY;
};