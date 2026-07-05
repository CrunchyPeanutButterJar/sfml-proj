#include <ecs/messaging/event_queue.hpp>

using namespace ecs::messaging;

void EventQueue::addEvent(EventId l_event)
{
    m_queue.push(l_event);
}

auto EventQueue::processEvent() -> std::optional<EventId>
{
    if (m_queue.empty())
    {
        return {};
    }
    auto event_id = m_queue.front();
    m_queue.pop();
    return event_id;
}

void EventQueue::clear()
{
    while (processEvent())
        ;
}