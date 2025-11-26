#include <string>
#include <functional>

#include "object_2D.h"
#include "text_renderer.h"
#include "sprite_renderer.h"

class MessageBox {
public:
    Object2D* background;
    Object2D* buttonArea;

    MessageBox(float width, float height);
    ~MessageBox();

    void Draw(SpriteRenderer& spriteRenderer, TextRenderer& textRenderer);
    void ProcessMouseClick(float x, float y);

    std::string getMessage() const;
    void setMessage(const std::string& message);
    void setMessage(const std::string& message, Texture2D& texture);

    bool isActive() const;

    // 设置确认按钮回调函数
    void setOnConfirmCallback(std::function<void()> callback);

private:
    std::string message;
    bool active;
    bool hasTexture;
    Texture2D sideTexture;

    std::function<void()> onConfirmCallback;

    float width;
    float height;

    void setActive(bool active);
    void setTexture(Texture2D& texture);
};