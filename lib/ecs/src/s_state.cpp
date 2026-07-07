#include "core/directions.hpp"
#include "ecs/entity/c_state.fwd.hpp"
#include "ecs/messaging/entity_events.hpp"
#include "ecs/messaging/entity_message.hpp"
#include "ecs/messaging/message.hpp"
#include "utils/bitmask.hpp"
#include <ecs/entity/c_state.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/system/s_state.hpp>
#include <ecs/system/system_manager.hpp>

namespace ecs::system
{

SState::SState(SystemManager& l_systemManager) : SBase{System::State, l_systemManager}
{
    utils::Bitmask req;
    req.turnOnBit((utils::Bitmask::Position)Component::State);
    m_requiredComponents.push_back(req);

    auto& msg_handler = m_systemManager.getMessageHandler();

    msg_handler.subscribe(messaging::EntityMessage::Move, this);
    msg_handler.subscribe(messaging::EntityMessage::Switch_State, this);
}

void SState::update(float /*l_dt*/)
{
    auto& entities = m_systemManager.getEntityManager();
    for (auto entity : m_entities)
    {
        auto*      state        = entities.getComponent<entity::CState>(entity, Component::State);
        const auto CurrentState = state->getState();
        if (CurrentState == entity::EntityState::Walking ||
            CurrentState == entity::EntityState::Running)
        {
            messaging::Message msg{
                .m_type     = (messaging::MessageType)messaging::EntityMessage::Is_Moving,
                .m_receiver = (int)entity,
                .m_bool     = false /*ignored*/
            };

            m_systemManager.getMessageHandler().dispatch(msg);
        }
    }
}

void SState::handleEvent(EntityId l_entity, messaging::EntityEvent l_event)
{
    switch (l_event)
    {
    case messaging::EntityEvent::Became_Idle:
    {
        changeState(l_entity, entity::EntityState::Idle, false);
        break;
    }
    default:
        break;
    }
}

void SState::notify(const messaging::Message& l_message)
{
    using namespace messaging;

    if (!hasEntity(l_message.m_receiver))
    {
        return;
    }

    const auto MsgType = (messaging::EntityMessage)l_message.m_type;

    switch (MsgType)
    {
    case EntityMessage::Move:
    {
        auto* state = m_systemManager.getEntityManager().getComponent<entity::CState>(
            l_message.m_receiver, Component::State);
        if (state->getState() == entity::EntityState::Dying)
        {
            return;
        }

        EntityEvent new_event{};
        const auto  Direction = (core::Direction)l_message.m_int;

        if (Direction == core::Direction::Right)
        {
            new_event = EntityEvent::Moving_Right;
        }
        else if (Direction == core::Direction::Left)
        {
            new_event = EntityEvent::Moving_Left;
        }

        ASSERT(l_message.m_receiver >= 0, "ill formed message, receiver is not valid {}",
               l_message.m_receiver);
        m_systemManager.addEvent(l_message.m_receiver, (EventId)new_event);
        changeState(l_message.m_receiver, entity::EntityState::Running, false);

        break;
    }

    case EntityMessage::Switch_State:
    {
        ASSERT(l_message.m_receiver >= 0, "ill formed message, receiver is not valid {}",
               l_message.m_receiver);
        changeState(l_message.m_receiver, (entity::EntityState)l_message.m_int, false);
        break;
    }

    default:
        break;
    }
}

void SState::changeState(EntityId l_entity, entity::EntityState l_state, bool l_force)
{
    auto& entities = m_systemManager.getEntityManager();
    auto* state    = entities.getComponent<entity::CState>(l_entity, Component::State);
    if (!l_force && state->getState() == entity::EntityState::Dying)
    {
        return;
    }

    state->setState(l_state);
    messaging::Message msg{.m_type =
                               (messaging::MessageType)messaging::EntityMessage::State_Changed,
                           .m_receiver = (int)l_entity,
                           .m_int      = (int)l_state};

    m_systemManager.getMessageHandler().dispatch(msg);
}

} // namespace ecs::system