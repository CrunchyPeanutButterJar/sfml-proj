#ifndef ECS_MESSAGING_EVENTQUEUE_HPP
#define ECS_MESSAGING_EVENTQUEUE_HPP

#include <queue>
#include <ecs/messaging/entityevents.hpp>
#include <optional>

using EventId = unsigned int;

class EventQueue
{
public:
    void addEvent(EventId l_event);
    std::optional<EventId> processEvent();
    void clear();

private:
    std::queue<EventId> m_queue;
};

#endif