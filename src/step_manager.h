#ifndef STEP_MANAGER_H
#define STEP_MANAGER_H

#include <vector>
#include <map>
#include <string>
#include "form.h"

struct Move {
    int from;
    int to;
};

class StepManager {
public:
    bool isRecording = false;

    void insert(int from, int to);
    bool record(const std::string& name);
    void endRecord();

    bool copy(const std::string& source, const std::string& newName);
    bool switchNum(const std::string& source, int raw, int target);

    // 新增的渲染函数
    void Render(SpriteRenderer& spriteRenderer, TextRenderer& textRenderer,
        glm::vec2 position, glm::vec2 size);

private:
    std::map<std::string, std::vector<Move>> data{};
    std::vector<Move> temp{};
    std::string currentRecordingName{};

};

#endif // !STEP_MANAGER_H