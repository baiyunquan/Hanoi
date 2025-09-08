#pragma once

#include <string>
#include <queue>

// �¼��ȼ�ö��
enum class EventLevel {
    HIGH,    // �߼��¼�
    MEDIUM,  // �м��¼�
    LOW      // �ͼ��¼�
};

// �¼��ṹ
struct GameEvent {
    std::string message;
    EventLevel level;
    float displayTime;

    GameEvent(const std::string& msg = "", EventLevel lvl = EventLevel::LOW, float time = 10.0f);
};

// �¼�������
class EventBus {
private:
    std::queue<GameEvent> highPriorityEvents;
    std::queue<GameEvent> mediumPriorityEvents;
    std::queue<GameEvent> lowPriorityEvents;
    GameEvent currentEvent;
    float remainingTime = 0.0f;

public:
    // ����¼�����Ӧ����
    void AddEvent(const GameEvent& event);

    // �����¼���ʾʱ�䲢��ȡ��ǰӦ��ʾ���¼�
    void Update(float dt);

    std::string GetCurrentMessage() const;

    // ��������¼�����
    void ClearAll();

    // ����Ƿ����¼�������ʾ
    bool IsDisplayingEvent() const;
    void AddHighPriorityEvent(const std::string& message, float time);
    void AddMediumPriorityEvent(const std::string& message, float time);
    void AddLowPriorityEvent(const std::string& message, float time);
};