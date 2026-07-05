#ifndef ECS_MESSAGING_EVENTQUEUE_HPP
#define ECS_MESSAGING_EVENTQUEUE_HPP

#include <ecs/messaging/entityevents.hpp>
#include <optional>
#include <queue>

namespace ecs::messaging
{
using EventId = unsigned int;

class EventQueue
{
  public:
    void addEvent(EventId l_event);
    auto processEvent() -> std::optional<EventId>;
    void clear();

  private:
    std::queue<EventId> m_queue;
};
} // namespace ecs::messaging

#endif