#include "step_manager.h"
#include "form.h"

bool StepManager::record(const std::string& name) {
    // ��������Ƿ�Ϊ�ջ��Ѵ���
    if (name.empty() || data.count(name) > 0) {
        return false;
    }

    currentRecordingName = name;
    isRecording = true;
    temp.clear(); // �����ʱ����
    return true;
}

void StepManager::endRecord() {
    if (isRecording && !currentRecordingName.empty()) {
        // �����¼�Ĳ���
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
    // ���Դ�Ƿ��������������Ч
    if (source == newName || data.count(source) == 0 ||
        data.count(newName) > 0 || newName.empty()) {
        return false;
    }

    // ��������
    data[newName] = data[source];
    return true;
}

bool StepManager::switchNum(const std::string& source, int raw, int target) {
    // ���Դ�Ƿ����
    if (data.count(source) == 0) {
        return false;
    }

    // ���������ƶ����裬�滻����
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

// ��������Ⱦ����ʵ��
void StepManager::Render(SpriteRenderer& spriteRenderer, TextRenderer& textRenderer,
    glm::vec2 position, glm::vec2 size) {
    // �����п�
    float keyWidth = size.x * 0.4f; // ������ռ40%���
    float valueWidth = size.x * 0.6f; // ��ť��ռ60%���

    // ��Ⱦ��ͷ
    float headerHeight = 30.0f;
    tableHeader.Render(spriteRenderer, textRenderer, headerHeight, keyWidth, valueWidth, position);

    // ��Ⱦ������
    float rowHeight = 30.0f;
    int rowIndex = 0;

    for (const auto& pair : data) {
        // ������ť����
        GameObject viewButton(glm::vec2(0, 0), 80, 25, glm::vec3(0.2f, 0.6f, 0.3f));
        GameObject switchButton(glm::vec2(0, 0), 80, 25, glm::vec3(0.2f, 0.3f, 0.6f));
        GameObject loadButton(glm::vec2(0, 0), 80, 25, glm::vec3(0.6f, 0.2f, 0.3f));

        viewButton.setText("View");
        switchButton.setText("Switch");
        loadButton.setText("Load");

        std::vector<GameObject> buttons = { viewButton, switchButton, loadButton };

        // �����ж�����Ⱦ
        Line line(pair.first, buttons);
        line.Render(spriteRenderer, textRenderer, rowHeight, keyWidth, valueWidth,
            glm::vec2(position.x, position.y + headerHeight + rowIndex * rowHeight));

        rowIndex++;
    }
}