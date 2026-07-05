#ifndef ECS_MESSAGING_OBSERVER_HPP
#define ECS_MESSAGING_OBSERVER_HPP

#include <ecs/messaging/message.hpp>

namespace ecs::messaging
{
class Observer
{
public:
    virtual ~Observer() = default;
    virtual void notify(const Message& l_message) = 0;
};
} // namespace ecs::messaging
#endif