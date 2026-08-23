#include "core/directions.hpp"
#include <ecs/ecs_types.hpp>
#include <ecs/entity/c_aicontroller.hpp>
#include <ecs/entity/c_movable.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/system/s_aicontrol.hpp>
#include <ecs/system/s_base.hpp>
#include <ecs/system/s_control.hpp>
#include <ecs/system/system_manager.hpp>
#include <utils/assert.hpp>
#include <utils/random.hpp>

namespace ecs::system
{
SAIControl::SAIControl(SystemManager& l_systemManager)
    : SBase{ecs::System::AIControl, l_systemManager}
{
    utils::Bitmask req;

    req.turnOnBit((utils::Bitmask::Position)Component::Position);
    req.turnOnBit((utils::Bitmask::Position)Component::Movable);
    req.turnOnBit((utils::Bitmask::Position)Component::AIController);
    req.turnOnBit((utils::Bitmask::Position)Component::Controller);

    m_requiredComponents.emplace_back(req);

    m_systemManager.getMessageHandler().subscribe(messaging::EntityMessage::Shift_Position, this);
}

void SAIControl::update(float /*l_dt*/)
{
    auto& msg_handler    = m_systemManager.getMessageHandler();
    auto& entity_manager = m_systemManager.getEntityManager();

    for (auto entity : m_entities)
    {
        constexpr float TOLERANCE = 5.F;

        auto* ai_control = entity_manager.getComponent<ecs::entity::CAIController>(
            entity, ecs::Component::AIController);
        auto* pos =
            entity_manager.getComponent<ecs::entity::CPosition>(entity, ecs::Component::Position);
        const auto Goal   = ai_control->getActiveGoal();
        const auto CurPos = pos->getPosition();

        if (std::fabs(Goal.x - CurPos.x) < TOLERANCE && std::fabs(Goal.y - CurPos.y) < TOLERANCE)
        {
            ai_control->switchToNextGoal();
        }
        else
        {
            if (Goal.x > CurPos.x)
            {
                moveEntity(msg_handler, entity, core::Direction::Right);
            }
            else if (Goal.x < CurPos.x)
            {
                moveEntity(msg_handler, entity, core::Direction::Left);
            }

            if (Goal.y > CurPos.y)
            {
                moveEntity(msg_handler, entity, core::Direction::Down);
            }
            else if (Goal.y < CurPos.y)
            {
                moveEntity(msg_handler, entity, core::Direction::Up);
            }
        }
    }
}

void SAIControl::handleEvent(EntityId /*l_entity*/, messaging::EntityEvent /*l_event*/) {}

void SAIControl::notify(const messaging::Message& l_message)
{
    switch ((messaging::EntityMessage)l_message.m_type)
    {
    case messaging::EntityMessage::Shift_Position:
    {
        auto& entities_manager = m_systemManager.getEntityManager();

        for (auto entity : m_entities)
        {
            auto* ai_controller = entities_manager.getComponent<ecs::entity::CAIController>(
                entity, Component::AIController);
            auto goals = ai_controller->getGoals();

            for (auto& goal : goals)
            {
                goal += l_message.m_2f;
            }

            ai_controller->setGoals(goals);
        }
        break;
    }

    default:
        break;
    }
}
} // namespace ecs::system