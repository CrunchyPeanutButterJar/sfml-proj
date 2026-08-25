#include "core/directions.hpp"
#include "ecs/entity/c_collidable.hpp"
#include <ecs/entity/c_state.fwd.hpp>
#include "ecs/messaging/entity_events.hpp"
#include "ecs/messaging/entity_message.hpp"
#include "ecs/messaging/event_queue.hpp"
#include <ecs/entity/c_movable.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/messaging/message_handler.hpp>
#include <ecs/system/s_control.hpp>
#include <ecs/system/system_manager.hpp>
#include <utils/utilities.hpp>

namespace ecs::system
{

SControl::SControl(SystemManager& l_systemManager) : SBase{System::Control, l_systemManager}
{
    utils::Bitmask req;

    req.turnOnBit((utils::Bitmask::Position)Component::Position);
    req.turnOnBit((utils::Bitmask::Position)Component::Movable);
    req.turnOnBit((utils::Bitmask::Position)Component::Controller);

    m_requiredComponents.emplace_back(req);

    m_systemManager.getMessageHandler().subscribe(messaging::EntityMessage::Jump, this);
}

void SControl::update(float /*l_dt*/) {}

void SControl::notify(const messaging::Message& l_message)
{
    const auto Type = (messaging::EntityMessage)l_message.m_type;
    switch (Type)
    {
    case messaging::EntityMessage::Jump:
    {
        auto entity = l_message.m_receiver;
        ASSERT_NON_FATAL(entity >= 0, "invalid entity id {}", entity);
        if (hasEntity(entity))
        {
            m_systemManager.addEvent(entity,
                                     (messaging::EventId)messaging::EntityEvent::Jump_Attempt);
        }

        break;
    }

    default:
    {
        break;
    }
    }
}

void SControl::handleEvent(EntityId l_entity, messaging::EntityEvent l_event)
{
    using namespace messaging;

    switch (l_event)
    {
    case EntityEvent::Moving_Left:
    {
        moveEntity(l_entity, core::Direction::Left);
        break;
    }

    case EntityEvent::Moving_Right:
    {
        moveEntity(l_entity, core::Direction::Right);
        break;
    }

    case EntityEvent::Moving_Up:
    {
        moveEntity(l_entity, core::Direction::Up);
        break;
    }

    case EntityEvent::Moving_down:
    {
        moveEntity(l_entity, core::Direction::Down);
        break;
    }

    case EntityEvent::Jump_Attempt:
    {
        auto& entities = m_systemManager.getEntityManager();
        auto* mov      = entities.getComponent<entity::CMovable>(l_entity, Component::Movable);
        if (auto* col = entities.getComponent<entity::CCollidable>(l_entity, Component::Collidable);
            col != nullptr && col->getGroundTile() != nullptr)
        {
            mov->jump();
            m_systemManager.addEvent(l_entity, (messaging::EventId)messaging::EntityEvent::Jumped);
        }
    };
    default:
        break;
    }
}

void notify(const messaging::Message& /*unused*/) {}

void SControl::moveEntity(EntityId l_entity, core::Direction l_dir)
{
    auto* mov = m_systemManager.getEntityManager().getComponent<entity::CMovable>(
        l_entity, Component::Movable);
    mov->move(l_dir);
}

void moveEntity(ecs::messaging::MessageHandler& l_messageHandler, ecs::EntityId l_entity,
                core::Direction l_dir)
{
    using namespace ecs::messaging;

    Message message{.m_type     = (MessageType)EntityMessage::Move,
                    .m_receiver = (int)l_entity,
                    .m_int      = (int)l_dir};

    l_messageHandler.dispatch(message);
}

void entityAttack(ecs::messaging::MessageHandler& l_messageHandler, ecs::EntityId l_entity)
{
    using namespace ecs::messaging;

    Message message{.m_type     = (MessageType)EntityMessage::Switch_State,
                    .m_receiver = (int)l_entity,
                    .m_int      = (int)entity::EntityState::Attacking};

    l_messageHandler.dispatch(message);
}

} // namespace ecs::system