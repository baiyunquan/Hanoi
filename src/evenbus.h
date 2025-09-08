#pragma once

#include <string>
#include <queue>

// 事件等级枚举
enum class EventLevel {
    HIGH,    // 高级事件
    MEDIUM,  // 中级事件
    LOW      // 低级事件
};

// 事件结构
struct GameEvent {
    std::string message;
    EventLevel level;
    float displayTime;

    GameEvent(const std::string& msg = "", EventLevel lvl = EventLevel::LOW, float time = 10.0f);
};

// 事件总线类
class EventBus {
private:
    std::queue<GameEvent> highPriorityEvents;
    std::queue<GameEvent> mediumPriorityEvents;
    std::queue<GameEvent> lowPriorityEvents;
    GameEvent currentEvent;
    float remainingTime = 0.0f;

public:
    // 添加事件到相应队列
    void AddEvent(const GameEvent& event);

    // 更新事件显示时间并获取当前应显示的事件
    void Update(float dt);

    std::string GetCurrentMessage() const;

    // 清空所有事件队列
    void ClearAll();

    // 检查是否有事件正在显示
    bool IsDisplayingEvent() const;
    void AddHighPriorityEvent(const std::string& message, float time);
    void AddMediumPriorityEvent(const std::string& message, float time);
    void AddLowPriorityEvent(const std::string& message, float time);
};