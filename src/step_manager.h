#ifndef STEP_MANAGER_H
#define STEP_MANAGER_H

#include <vector>
#include <map>
#include <string>
#include <algorithm> // 需要包含algorithm头文件
#include <iterator>  // 需要包含iterator头文件
#include "form.h"

struct Move {
    int from;
    int to;
};

class StepManager {
public:
    StepManager();

    bool isRecording = false;
    Form* form;

    void insert(int from, int to);
    bool record(const std::string& name);
    void endRecord();

    bool copy(const std::string& source, const std::string& newName);
    bool switchNum(const std::string& source, int raw, int target);

    // 新增的渲染函数
    void Render(SpriteRenderer& renderer, TextRenderer& textRenderer, float screenWidth, float screenHeight);

    void onMouseReleased(float x, float y);
    std::string movesToString(const std::vector<Move>& moves);

    void regViewCall(std::function<void(std::string &)> callback);
    void regSwCall(std::function<void(std::string&)> callback);
    void regLoadCall(std::function<void(std::vector<Move>*)> callback);
private:
    std::map<std::string, std::vector<Move>> data{};
    std::vector<Move> temp{};
    std::string currentRecordingName{};

    // void callback( keyStr , valueStr)
    std::function<void(std::string&)> viewCallBack;
    std::function<void(std::string&)> switchCallBack;
    std::function<void(std::vector<Move>*)> loadCallBack;
};

#endif // !STEP_MANAGER_H