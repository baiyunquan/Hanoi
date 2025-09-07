#include <string>
#include <functional>

#include "game_object.h"
#include "text_renderer.h"
#include "sprite_renderer.h"

class TextInput {
public:
    GameObject* background;
    GameObject* inputArea;

    TextInput(float width, float height);
    ~TextInput();

    void Draw(SpriteRenderer& r1, TextRenderer& r2);
    void ProcessKey(int key, int action);
    std::string getResult();
    void clear();
    bool isActive() const;
    void setActive(bool active);

    // 添加回调函数用于处理输入完成
    void setOnSubmitCallback(std::function<void(const std::string&)> callback);

private:
    std::string content;
    bool active;
    std::function<void(const std::string&)> onSubmitCallback;

    float width;
    float height;
};