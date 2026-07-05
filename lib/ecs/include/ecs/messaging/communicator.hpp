#ifndef COMMUNICATOR_HPP
#define COMMUNICATOR_HPP

#include <ecs/messaging/observer.hpp>
#include <set>

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

#endif