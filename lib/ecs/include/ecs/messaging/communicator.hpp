#ifndef ECS_MESSAGING_COMMUNICATOR_HPP
#define ECS_MESSAGING_COMMUNICATOR_HPP

#include <ecs/messaging/observer.hpp>
#include <set>

namespace ecs::messaging
{
using ObserverContainer = std::set<Observer*>;

class Communicator
{
public:
    auto addObserver(Observer* l_observer) -> bool;
    auto removeObserver(Observer* l_observer) -> bool;
    auto hasObserver(const Observer* l_observer) const -> bool;
    void broadcast(const Message& l_message) const;

private:
    ObserverContainer m_observers;
};
} // namespace ecs::messaging

#endif