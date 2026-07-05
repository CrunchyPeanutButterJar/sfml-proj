#ifndef ECS_MESSAGING_MESSAGEHANDLER_HPP
#define ECS_MESSAGING_MESSAGEHANDLER_HPP

#include <ecs/messaging/entitymessage.hpp>
#include <ecs/messaging/message.hpp>
#include <ecs/messaging/communicator.hpp>
#include <unordered_map>

using Subscriptions = std::unordered_map<EntityMessage, Communicator>;

class MessageHandler
{
public:
    bool subscribe(EntityMessage l_type, Observer* l_observer);
    bool unsubscribe(EntityMessage l_type, Observer* l_observer);
    void dispatch(const Message& l_message) const;

private:
    Subscriptions m_communicators;
};

#endif