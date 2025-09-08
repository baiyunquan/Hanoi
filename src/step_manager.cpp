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

    std::vector<std::string> keys;
    // 使用std::transform算法
    std::transform(data.begin(), data.end(), std::back_inserter(keys),
        [](const auto& pair) { return pair.first; });
    form->updateData(keys);
}

StepManager::StepManager()
{
    std::vector<std::string> valueNames = { "View", "Switch", "Load" };
    form = new Form("Memory Name", valueNames);
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
        else {
            if (move.from == target) {
                move.from = raw;
            }
        }

        if (move.to == raw) {
            move.to = target;
        }
        else {
            if (move.to == target) {
                move.to = raw;
            }
        }
    }

    return true;
}

// 新增的渲染函数实现
void StepManager::Render(SpriteRenderer& spriteRenderer, TextRenderer& textRenderer,
    float width , float height) {
    form->Render(spriteRenderer, textRenderer, width, height);
}

void StepManager::onMouseReleased(float x, float y) {
    for (Line line : form->lines) {
        auto& view = line.values[0];
        if (view.isChosen(x, y)) {
            if(viewCallBack)
                this->viewCallBack(movesToString(data[line.key]));
        }

        auto& sw = line.values[1];
        if (sw.isChosen(x, y)) {
            if (switchCallBack)
                this->switchCallBack(line.key);
        }
        auto& load = line.values[2];
        if (load.isChosen(x, y)) {

        }
    }
}

std::string StepManager::movesToString(const std::vector<Move>& moves) {
    std::ostringstream oss;

    for (size_t i = 0; i < moves.size(); ++i) {
        if (i > 0) {
            oss << " , ";
        }
        oss << moves[i].from << "->" << moves[i].to;
    }

    return oss.str();
}


void StepManager::regViewCall(std::function<void(std::string&)> callback)
{
    viewCallBack = callback;
}

void StepManager::regSwCall(std::function<void(std::string&)> callback)
{
    switchCallBack = callback;
}

void StepManager::regLoadCall(std::function<void(std::vector<Move>*)> callback)
{
    loadCallBack = callback;
}
