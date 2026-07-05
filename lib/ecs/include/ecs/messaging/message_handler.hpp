#ifndef ECS_MESSAGING_MESSAGE_HANDLER_HPP
#define ECS_MESSAGING_MESSAGE_HANDLER_HPP

#include <ecs/messaging/communicator.hpp>
#include <ecs/messaging/entity_message.hpp>
#include <ecs/messaging/message.hpp>
#include <unordered_map>

namespace ecs::messaging
{
using Subscriptions = std::unordered_map<EntityMessage, Communicator>;

class MessageHandler
{
  public:
    auto subscribe(EntityMessage l_type, Observer* l_observer) -> bool;
    auto unsubscribe(EntityMessage l_type, Observer* l_observer) -> bool;
    void dispatch(const Message& l_message) const;

  private:
    Subscriptions m_communicators;
};
} // namespace ecs::messaging
#endif