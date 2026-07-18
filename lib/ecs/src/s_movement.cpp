#include "core/directions.hpp"
#include "ecs/ecs_types.hpp"
#include "ecs/entity/c_collidable.hpp"
#include "ecs/entity/c_state.fwd.hpp"
#include "ecs/messaging/entity_events.hpp"
#include "ecs/messaging/entity_message.hpp"
#include "ecs/messaging/event_queue.hpp"
#include "ecs/messaging/message.hpp"
#include "ecs/messaging/message_handler.hpp"
#include "utils/assert.hpp"
#include <SFML/System/Vector2.hpp>
#include <core/graphics/tiles.hpp>
#include <ecs/entity/c_movable.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/map.hpp>
#include <ecs/system/s_movement.hpp>
#include <ecs/system/system_manager.hpp>
#include <utils/bitmask.hpp>

namespace ecs::system
{
SMovement::SMovement(SystemManager& l_system_manager) : SBase{System::Movement, l_system_manager}
{
    utils::Bitmask req;
    req.turnOnBit((utils::Bitmask::Position)Component::Position);
    req.turnOnBit((utils::Bitmask::Position)Component::Movable);
    m_requiredComponents.emplace_back(req);
}

void SMovement::update(float l_dt)
{
    if (m_map == nullptr)
    {
        return;
    }

    auto& entities = m_systemManager.getEntityManager();
    for (auto entity : m_entities)
    {
        auto* position = entities.getComponent<entity::CPosition>(entity, ecs::Component::Position);
        auto* movable  = entities.getComponent<entity::CMovable>(entity, ecs::Component::Movable);
        const core::graphics::Tile* ground_tile = nullptr;

        if (auto* collidable =
                entities.getComponent<entity::CCollidable>(entity, ecs::Component::Collidable))
        {
            ground_tile = collidable->getGroundTile();
        }

        movementStep(l_dt, movable, ground_tile, m_map->getGravity());
        position->moveBy(movable->getVelocity() * l_dt);
    }
}

void SMovement::setMap(Map* l_map)
{
    m_map = l_map;
}

void SMovement::movementStep(float l_dt, entity::CMovable* l_movable,
                             const core::graphics::Tile* l_tile, float l_gravity)
{
    static const sf::Vector2f DefaultFriction{5, 0.};

    l_movable->accelerate({0.F, l_gravity});
    l_movable->addVelocity(l_movable->getAcceleration() * l_dt);
    if (l_tile != nullptr)
    {
        l_movable->applyFriction(l_tile->m_tileInfo->m_friction);
    }
    else
    {
        l_movable->applyFriction(DefaultFriction);
    }
    l_movable->setAcceleration({0., 0.});

    // const auto [VX, VY]    = l_movable->getVelocity();
    // const auto MaxVelocity = l_movable->getMaxVelocity();

    // float magnitude = sqrt(VX * VX + VY * VY);
    // if (magnitude <= MaxVelocity)
    // {
    //     return;
    // }

    // const auto VXMax = MaxVelocity * VX / magnitude;
    // const auto VYMax = MaxVelocity * VY / magnitude;

    // l_movable->setVelocity({VXMax, VYMax});
}

void SMovement::stopEntity(EntityId l_entity, Axis l_axis)
{
    auto* mov = m_systemManager.getEntityManager().getComponent<entity::CMovable>(
        l_entity, Component::Movable);
    if (l_axis == Axis::x)
    {
        mov->setVelocity({0., mov->getVelocity().y});
    }
    else if (l_axis == Axis::y)
    {
        mov->setVelocity({mov->getVelocity().x, 0.});
    }
}

void SMovement::setDirection(EntityId l_entity, core::Direction l_dir)
{
    auto* mov = m_systemManager.getEntityManager().getComponent<entity::CMovable>(
        l_entity, Component::Movable);
    mov->setDirection(l_dir);

    messaging::Message msg{.m_type =
                               (messaging::MessageType)messaging::EntityMessage::Direction_Changed,
                           .m_receiver = (int)l_entity,
                           .m_int      = (int)l_dir};

    m_systemManager.getMessageHandler().dispatch(msg);
}

void SMovement::notify(const messaging::Message& /*l_message*/) {}

void SMovement::handleEvent(EntityId l_entity, messaging::EntityEvent l_event)
{
    using namespace messaging;

    switch (l_event)
    {
    case EntityEvent::Colliding_X:
    {
        stopEntity(l_entity, Axis::x);
    }
    break;
    case EntityEvent::Colliding_Y:
    {
        stopEntity(l_entity, Axis::y);
    }
    break;
    case EntityEvent::Moving_Right:
    {
        setDirection(l_entity, core::Direction::Right);
    }
    break;
    case EntityEvent::Moving_Left:
    {
        setDirection(l_entity, core::Direction::Left);
        break;
    }
    case EntityEvent::Not_Grounded:
    {
        if (hasEntity(l_entity))
        {
            auto* movable = m_systemManager.getEntityManager().getComponent<entity::CMovable>(
                l_entity, Component::Movable);
            if (movable->getVelocity().y > 0)
            {
                m_systemManager.addEvent(l_entity, (EventId)EntityEvent::Falling);
            }
        }
        break;
    }
    case EntityEvent::Is_Moving:
    {
        if (!hasEntity(l_entity))
        {
            return;
        }
        auto& entities = m_systemManager.getEntityManager();
        auto* movable  = entities.getComponent<entity::CMovable>(l_entity, Component::Movable);
        if (movable->getVelocity() != sf::Vector2f{0., 0.})
        {
            return;
        }
        m_systemManager.addEvent(l_entity, (EventId)EntityEvent::Became_Idle);
    }
    break;
    default:
        break;
    }
}

} // namespace ecs::system