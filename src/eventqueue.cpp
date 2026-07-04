#include <eventqueue.hpp>

void EventQueue::addEvent(EventId l_event)
{
    m_queue.push(l_event);
}

std::optional<EventId> EventQueue::processEvent()
{
    if(m_queue.empty())
    {
        return {};
    }
    auto eventId = m_queue.front();
    m_queue.pop();
    return eventId;
}

void EventQueue::clear()
{
    while(processEvent());
}