#include <ecs/messaging/messagehandler.hpp>

using namespace ecs::messaging;

bool MessageHandler::subscribe(EntityMessage l_type, Observer* l_observer)
{
    return m_communicators[l_type].addObserver(l_observer);
}

bool MessageHandler::unsubscribe(EntityMessage l_type, Observer* l_observer)
{
    return m_communicators[l_type].removeObserver(l_observer);
}

void MessageHandler::dispatch(const Message& l_message) const
{
    auto type = (EntityMessage)l_message.m_type;
    auto itr = m_communicators.find(type);
    if (itr != m_communicators.end())
    {
        itr->second.broadcast(l_message);
    }
}