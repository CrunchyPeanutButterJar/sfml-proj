#include "core/directions.hpp"
#include "ecs/entity/c_collidable.hpp"
#include "ecs/messaging/entity_events.hpp"
#include "ecs/messaging/entity_message.hpp"
#include "ecs/messaging/event_queue.hpp"
#include <ecs/entity/c_movable.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/system/s_control.hpp>
#include <ecs/system/system_manager.hpp>

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

} // namespace ecs::system