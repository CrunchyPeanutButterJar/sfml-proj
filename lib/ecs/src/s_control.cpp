#include "core/directions.hpp"
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
}

void SControl::update(float /*l_dt*/) {}

void SControl::notify(const messaging::Message& /*l_message*/) {}

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