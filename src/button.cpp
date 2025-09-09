#include "button.h"

void Button::StartBounceAnimation(float duration)
{
    bounceState = SHRINKING;
    bounceProgress = 0.0f;
    bounceDuration = duration;
    originalSizeX = SizeX;
    originalSizeY = SizeY;
}

void Button::UpdateBounceAnimation(float dt)
{
    if (bounceState == NONE) return;

    bounceProgress += dt / bounceDuration;

    if (bounceState == SHRINKING)
    {
        // 缩小阶段 (0% - 50%)
        float progress = bounceProgress * 2.0f; // 映射到 0-1
        if (progress > 1.0f) progress = 1.0f;

        // 使用二次缓动函数实现平滑缩小，缩小到原来的80%
        float scale = 1.0f - progress * progress * 0.2f; // 缩小到80%
        SizeX = originalSizeX * scale;
        SizeY = originalSizeY * scale;

        if (bounceProgress >= 0.5f)
        {
            bounceState = EXPANDING;
            bounceProgress = 0.5f; // 重置进度，从50%开始
        }
    }
    else if (bounceState == EXPANDING)
    {
        // 扩大阶段 (50% - 100%)
        float progress = (bounceProgress - 0.5f) * 2.0f; // 映射到 0-1
        if (progress > 1.0f) progress = 1.0f;

        // 使用弹性函数实现有弹性的扩大效果，从80%扩大到100%
        float scale = 0.8f + progress * progress * (1.2f - progress); // 从80%扩大到100%
        SizeX = originalSizeX * scale;
        SizeY = originalSizeY * scale;

        if (bounceProgress >= 1.0f)
        {
            bounceState = NONE;
            // 确保最终恢复到原始大小
            SizeX = originalSizeX;
            SizeY = originalSizeY;
        }
    }
}

void Button::Draw(SpriteRenderer& renderer)
{
    // 移除颜色变化效果，始终使用原始颜色
    glm::vec3 drawColor = this->Color;

    if (isRect) {
        renderer.DrawRectangle(this->Position, glm::vec2(this->SizeX, this->SizeY), this->Rotation, drawColor);
        return;
    }
    renderer.DrawSprite(this->Sprite, this->Position, glm::vec2(this->SizeX, this->SizeY), this->Rotation, drawColor);
}