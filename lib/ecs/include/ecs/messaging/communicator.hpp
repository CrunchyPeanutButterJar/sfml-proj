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
    bool addObserver(Observer* l_observer);
    bool removeObserver(Observer* l_observer);
    bool hasObserver(const Observer* l_observer) const;
    void broadcast(const Message& l_message) const;

private:
    ObserverContainer m_observers;
};
};

#endif