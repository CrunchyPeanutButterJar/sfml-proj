#ifndef OBSERVER_HPP
#define OBSERVER_HPP

#include <ecs/messaging/message.hpp>

class Observer
{
public:
    virtual ~Observer() = default;
    virtual void notify(const Message& l_message) = 0;
};

#endif