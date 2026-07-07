#include "core/directions.hpp"
#include "ecs/ecs_types.hpp"
#include "ecs/entity/c_state.fwd.hpp"
#include "ecs/messaging/entity_events.hpp"
#include "ecs/messaging/entity_message.hpp"
#include "ecs/messaging/message.hpp"
#include "ecs/messaging/message_handler.hpp"
#include <SFML/System/Vector2.hpp>
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

    m_systemManager.getMessageHandler().subscribe(messaging::EntityMessage::Is_Moving, this);
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
        movementStep(l_dt, movable, position);
        position->moveBy(movable->getVelocity() * l_dt);
    }
}

void SMovement::setMap(Map* l_map)
{
    m_map = l_map;
}

void SMovement::movementStep(float l_dt, entity::CMovable* l_movable,
                             entity::CPosition* /*l_position*/)
{
    l_movable->addVelocity(l_movable->getAcceleration() * l_dt);
    // TODO: account for friction
    l_movable->applyFriction({10., 0});
    l_movable->setAcceleration({0., 0.});

    const auto [VX, VY]    = l_movable->getVelocity();
    const auto MaxVelocity = l_movable->getMaxVelocity();

    float magnitude = sqrt(VX * VX + VY * VY);
    if (magnitude <= MaxVelocity)
    {
        return;
    }

    const auto VXMax = MaxVelocity * VX / magnitude;
    const auto VYMax = MaxVelocity * VY / magnitude;

    l_movable->setVelocity({VXMax, VYMax});
}

auto SMovement::getTileFriction(size_t iRow, size_t iCol) -> const sf::Vector2f&
{
    static const sf::Vector2f DefaultFriction{0., 0.};

    const auto* tile = m_map->getTile(iRow, iCol);
    if (tile == nullptr)
    {
        return DefaultFriction;
    }

    return tile->m_tileInfo->m_friction;
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

void SMovement::notify(const messaging::Message& l_message)
{
    using namespace messaging;

    auto&      entities    = m_systemManager.getEntityManager();
    const auto MessageType = (EntityMessage)l_message.m_type;

    switch (MessageType)
    {
    case EntityMessage::Is_Moving:
    {
        if (!hasEntity(l_message.m_receiver))
        {
            return;
        }
        auto* movable =
            entities.getComponent<entity::CMovable>(l_message.m_receiver, Component::Movable);
        if (movable->getVelocity() != sf::Vector2f{0., 0.})
        {
            return;
        }
        m_systemManager.addEvent(l_message.m_receiver, (EventId)EntityEvent::Became_Idle);
    }
    break;
    default:
        break;
    }
}

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
    }
    break;
    default:
        break;
    }
}

} // namespace ecs::system