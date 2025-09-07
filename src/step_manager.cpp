#include "step_manager.h"

bool StepManager::record(const std::string& name) {
    
    // ��������Ƿ���Ч�Ҳ�����
    if (name.empty() || data.count(name) > 0) {
        return false;
    }
    int i = data.count(name);

    currentRecordingName = name;
    isRecording = true;
    temp.clear(); // �����ʱ�洢
    return true;
}

void StepManager::endRecord() {
    if (isRecording && !currentRecordingName.empty()) {
        // ����¼�Ƶ�����
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

    // ���������ƶ����裬�滻ָ��������
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