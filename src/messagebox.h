#include <string>
#include <functional>

#include "game_object.h"
#include "text_renderer.h"
#include "sprite_renderer.h"

class MessageBox {
public:
    GameObject* background;
    GameObject* buttonArea;

    MessageBox(float width, float height);
    ~MessageBox();

    void Draw(SpriteRenderer& spriteRenderer, TextRenderer& textRenderer);
    void ProcessMouseClick(float x, float y);

    std::string getMessage() const;
    void setMessage(const std::string& message);

    bool isActive() const;
    void setActive(bool active);

    // 设置确认按钮回调函数
    void setOnConfirmCallback(std::function<void()> callback);

private:
    std::string message;
    bool active;
    std::function<void()> onConfirmCallback;

    float width;
    float height;

};