#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include <entitymessage.hpp>
#include <message.hpp>
#include <communicator.hpp>
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