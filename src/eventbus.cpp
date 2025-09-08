#include "evenbus.h"

// GameEvent ���캯��
GameEvent::GameEvent(const std::string& msg, EventLevel lvl, float time)
    : message(msg), level(lvl), displayTime(time) {
}

// ����¼�����Ӧ����
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

// �����¼���ʾʱ��
void EventBus::Update(float dt) {
    if (remainingTime > 0.0f) {
        remainingTime -= dt;
    }

    // �����ǰ�¼��ѽ�����û�е�ǰ�¼�����ȡ��һ���¼�
    if (remainingTime <= 0.0f) {
        // ���ȴӸ����ȼ����л�ȡ�¼�
        if (!highPriorityEvents.empty()) {
            currentEvent = highPriorityEvents.front(); // �����¼������ǻ�ȡָ��
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
            currentEvent = GameEvent(); // ����Ϊ���¼�
            remainingTime = 0.0f;
        }
    }
}

// ��ȡ��ǰ�¼���Ϣ
std::string EventBus::GetCurrentMessage() const {
    return currentEvent.message;
}

// ��������¼�����
void EventBus::ClearAll() {
    while (!highPriorityEvents.empty()) highPriorityEvents.pop();
    while (!mediumPriorityEvents.empty()) mediumPriorityEvents.pop();
    while (!lowPriorityEvents.empty()) lowPriorityEvents.pop();
    currentEvent = GameEvent();
    remainingTime = 0.0f;
}

// ����Ƿ����¼�������ʾ
bool EventBus::IsDisplayingEvent() const {
    return remainingTime > 0.0f && !currentEvent.message.empty();
}

// ��������¼��ķ���ʾ��
void EventBus::AddHighPriorityEvent(const std::string& message , float time) {
    this->AddEvent(GameEvent(message, EventLevel::HIGH , time));
}

void EventBus::AddMediumPriorityEvent(const std::string& message, float time) {
    this->AddEvent(GameEvent(message, EventLevel::MEDIUM , time));
}

void EventBus::AddLowPriorityEvent(const std::string& message, float time) {
    this->AddEvent(GameEvent(message, EventLevel::LOW , time));
}