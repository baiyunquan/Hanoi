#pragma once

#include <vector>
#include <functional>
#include "step_manager.h"

class Timer {
public:
    std::vector<Move>* temp;
    int step = 0;
    float accumulator = 0.0f;  // �����ۻ�ʱ��
    bool isRunning = false;    // ��ʱ������״̬

    void init(std::vector<Move>* ptr) {
        temp = ptr;
        reset();
    }

    void reset() {
        step = 0;
        accumulator = 0.0f;
        isRunning = (temp != nullptr && !temp->empty());
    }

    bool update(float dt) {
        if (!isRunning) return false;

        accumulator += dt;

        // ÿ2�봦��һ��Move
        if (accumulator >= 2.0f) {
            accumulator = 0.0f;  // �����ۻ�ʱ��

            if (step < temp->size()) {
                if (callBack) {
                    this->callBack((*temp)[step]);
                }
                step++;
            }

            // ����Ƿ�����Move���Ѵ���
            if (step >= temp->size()) {
                isRunning = false;
                return false;
            }
            return true;
        }
        return false;
    }

    void setCallBack(std::function<void(Move)> func) {
        callBack = func;
    }


private :
    std::function<void(Move)> callBack;
};