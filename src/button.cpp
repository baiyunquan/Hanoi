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
        // ��С�׶� (0% - 50%)
        float progress = bounceProgress * 2.0f; // ӳ�䵽 0-1
        if (progress > 1.0f) progress = 1.0f;

        // ʹ�ö��λ�������ʵ��ƽ����С����С��ԭ����80%
        float scale = 1.0f - progress * progress * 0.2f; // ��С��80%
        SizeX = originalSizeX * scale;
        SizeY = originalSizeY * scale;

        if (bounceProgress >= 0.5f)
        {
            bounceState = EXPANDING;
            bounceProgress = 0.5f; // ���ý��ȣ���50%��ʼ
        }
    }
    else if (bounceState == EXPANDING)
    {
        // ����׶� (50% - 100%)
        float progress = (bounceProgress - 0.5f) * 2.0f; // ӳ�䵽 0-1
        if (progress > 1.0f) progress = 1.0f;

        // ʹ�õ��Ժ���ʵ���е��Ե�����Ч������80%����100%
        float scale = 0.8f + progress * progress * (1.2f - progress); // ��80%����100%
        SizeX = originalSizeX * scale;
        SizeY = originalSizeY * scale;

        if (bounceProgress >= 1.0f)
        {
            bounceState = NONE;
            // ȷ�����ջָ���ԭʼ��С
            SizeX = originalSizeX;
            SizeY = originalSizeY;
        }
    }
}

void Button::Draw(SpriteRenderer& renderer)
{
    // �Ƴ���ɫ�仯Ч����ʼ��ʹ��ԭʼ��ɫ
    glm::vec3 drawColor = this->Color;

    if (isRect) {
        renderer.DrawRectangle(this->Position, glm::vec2(this->SizeX, this->SizeY), this->Rotation, drawColor);
        return;
    }
    renderer.DrawSprite(this->Sprite, this->Position, glm::vec2(this->SizeX, this->SizeY), this->Rotation, drawColor);
}