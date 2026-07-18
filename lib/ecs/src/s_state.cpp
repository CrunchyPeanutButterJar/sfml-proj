#include "core/directions.hpp"
#include "ecs/entity/c_state.fwd.hpp"
#include "ecs/messaging/entity_events.hpp"
#include "ecs/messaging/entity_message.hpp"
#include "ecs/messaging/message.hpp"
#include "utils/bitmask.hpp"
#include <ecs/entity/c_state.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/messaging/event_queue.hpp>
#include <ecs/system/s_state.hpp>
#include <ecs/system/system_manager.hpp>
#include <unordered_map>
#include <utils/utilities.hpp>

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
            CurrentState == entity::EntityState::Running ||
            CurrentState == entity::EntityState::Jumping ||
            CurrentState == entity::EntityState::Falling ||
            CurrentState == entity::EntityState::Landing)
        {
            m_systemManager.addEvent(entity, (messaging::EventId)messaging::EntityEvent::Is_Moving);
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
    case messaging::EntityEvent::Jumped:
    {
        changeState(l_entity, entity::EntityState::Jumping, false);
        break;
    }
    case messaging::EntityEvent::Falling:
    {
        changeState(l_entity, entity::EntityState::Falling, false);
        break;
    }
    case messaging::EntityEvent::Colliding_Y:
    {
        changeState(l_entity, entity::EntityState::Landing, false);
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

static auto getTransitionsGraph()
{
    std::array<std::array<bool, (size_t)entity::EntityState::Count>,
               (size_t)entity::EntityState::Count>
        transitions{};

    for (size_t i = 0; i < (size_t)entity::EntityState::Count; i++)
    {
        transitions[i][(size_t)entity::EntityState::Dying] = true;
    }

    transitions[(size_t)entity::EntityState::Idle][(size_t)entity::EntityState::Running] = true;
    transitions[(size_t)entity::EntityState::Idle][(size_t)entity::EntityState::Walking] = true;
    transitions[(size_t)entity::EntityState::Idle][(size_t)entity::EntityState::Jumping] = true;

    transitions[(size_t)entity::EntityState::Running][(size_t)entity::EntityState::Idle]    = true;
    transitions[(size_t)entity::EntityState::Running][(size_t)entity::EntityState::Walking] = true;
    transitions[(size_t)entity::EntityState::Running][(size_t)entity::EntityState::Jumping] = true;
    transitions[(size_t)entity::EntityState::Running][(size_t)entity::EntityState::Falling] = true;

    transitions[(size_t)entity::EntityState::Jumping][(size_t)entity::EntityState::Falling] = true;

    transitions[(size_t)entity::EntityState::Falling][(size_t)entity::EntityState::Landing] = true;

    transitions[(size_t)entity::EntityState::Landing][(size_t)entity::EntityState::Running] = true;
    transitions[(size_t)entity::EntityState::Landing][(size_t)entity::EntityState::Walking] = true;
    transitions[(size_t)entity::EntityState::Landing][(size_t)entity::EntityState::Jumping] = true;

    return transitions;
}

static auto transitionIsValid(entity::EntityState l_prevState,
                              entity::EntityState l_newState) -> bool
{
    static const auto Transitions = getTransitionsGraph();

    return Transitions[(size_t)l_prevState][(size_t)l_newState];
}

void SState::changeState(EntityId l_entity, entity::EntityState l_state, bool l_force)
{
    auto& entities = m_systemManager.getEntityManager();
    auto* state    = entities.getComponent<entity::CState>(l_entity, Component::State);
    if (!l_force && !transitionIsValid(state->getState(), l_state))
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