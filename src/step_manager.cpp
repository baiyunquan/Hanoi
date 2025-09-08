#include "step_manager.h"
#include "form.h"

bool StepManager::record(const std::string& name) {
    // 检查名称是否为空或已存在
    if (name.empty() || data.count(name) > 0) {
        return false;
    }

    currentRecordingName = name;
    isRecording = true;
    temp.clear(); // 清除临时数据
    return true;
}

void StepManager::endRecord() {
    if (isRecording && !currentRecordingName.empty()) {
        // 保存记录的步骤
        data[currentRecordingName] = temp;
    }

    isRecording = false;
    currentRecordingName.clear();
    temp.clear();
}

void StepManager::insert(int from, int to) {
    if (isRecording) {
        temp.push_back({ from, to });
    }
}

bool StepManager::copy(const std::string& source, const std::string& newName) {
    // 检查源是否存在且新名称有效
    if (source == newName || data.count(source) == 0 ||
        data.count(newName) > 0 || newName.empty()) {
        return false;
    }

    // 复制数据
    data[newName] = data[source];
    return true;
}

bool StepManager::switchNum(const std::string& source, int raw, int target) {
    // 检查源是否存在
    if (data.count(source) == 0) {
        return false;
    }

    // 遍历所有移动步骤，替换数字
    for (auto& move : data[source]) {
        if (move.from == raw) {
            move.from = target;
        }
        if (move.to == raw) {
            move.to = target;
        }
    }

    return true;
}

// 新增的渲染函数实现
void StepManager::Render(SpriteRenderer& spriteRenderer, TextRenderer& textRenderer,
    glm::vec2 position, glm::vec2 size) {
    
    
}