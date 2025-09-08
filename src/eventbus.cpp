#include "evenbus.h"

// GameEvent 构造函数
GameEvent::GameEvent(const std::string& msg, EventLevel lvl, float time)
    : message(msg), level(lvl), displayTime(time) {
}

// 添加事件到相应队列
void EventBus::AddEvent(const GameEvent& event) {
    switch (event.level) {
    case EventLevel::HIGH:
        highPriorityEvents.push(event);
        break;
    case EventLevel::MEDIUM:
        mediumPriorityEvents.push(event);
        break;
    case EventLevel::LOW:
        lowPriorityEvents.push(event);
        break;
    }
}

// 更新事件显示时间
void EventBus::Update(float dt) {
    if (remainingTime > 0.0f) {
        remainingTime -= dt;
    }

    // 如果当前事件已结束或没有当前事件，获取下一个事件
    if (remainingTime <= 0.0f) {
        // 优先从高优先级队列获取事件
        if (!highPriorityEvents.empty()) {
            currentEvent = highPriorityEvents.front(); // 复制事件而不是获取指针
            highPriorityEvents.pop();
            remainingTime = currentEvent.displayTime;
        }
        else if (!mediumPriorityEvents.empty()) {
            currentEvent = mediumPriorityEvents.front();
            mediumPriorityEvents.pop();
            remainingTime = currentEvent.displayTime;
        }
        else if (!lowPriorityEvents.empty()) {
            currentEvent = lowPriorityEvents.front();
            lowPriorityEvents.pop();
            remainingTime = currentEvent.displayTime;
        }
        else {
            currentEvent = GameEvent(); // 重置为空事件
            remainingTime = 0.0f;
        }
    }
}

// 获取当前事件消息
std::string EventBus::GetCurrentMessage() const {
    return currentEvent.message;
}

// 清空所有事件队列
void EventBus::ClearAll() {
    while (!highPriorityEvents.empty()) highPriorityEvents.pop();
    while (!mediumPriorityEvents.empty()) mediumPriorityEvents.pop();
    while (!lowPriorityEvents.empty()) lowPriorityEvents.pop();
    currentEvent = GameEvent();
    remainingTime = 0.0f;
}

// 检查是否有事件正在显示
bool EventBus::IsDisplayingEvent() const {
    return remainingTime > 0.0f && !currentEvent.message.empty();
}

// 添加其他事件的方法示例
void EventBus::AddHighPriorityEvent(const std::string& message , float time) {
    this->AddEvent(GameEvent(message, EventLevel::HIGH , time));
}

void EventBus::AddMediumPriorityEvent(const std::string& message, float time) {
    this->AddEvent(GameEvent(message, EventLevel::MEDIUM , time));
}

void EventBus::AddLowPriorityEvent(const std::string& message, float time) {
    this->AddEvent(GameEvent(message, EventLevel::LOW , time));
}