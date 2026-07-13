#include "ecs/ecs_types.hpp"
#include "ecs/entity/c_state.fwd.hpp"
#include "ecs/messaging/entity_message.hpp"
#include "utils/bitmask.hpp"
#include <ecs/entity/c_spritesheet.hpp>
#include <ecs/entity/c_state.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/system/s_sheet_animation.hpp>
#include <ecs/system/system_manager.hpp>

namespace ecs::system
{
SSheetAnimation::SSheetAnimation(SystemManager& l_systemManager)
    : SBase{System::SheetAnimation, l_systemManager}
{
    utils::Bitmask req;
    req.turnOnBit((utils::Bitmask::Position)Component::SpriteSheet);
    req.turnOnBit((utils::Bitmask::Position)Component::State);
    m_requiredComponents.emplace_back(req);

    m_systemManager.getMessageHandler().subscribe(messaging::EntityMessage::State_Changed, this);
}

void SSheetAnimation::update(float l_dt)
{
    auto& entities = m_systemManager.getEntityManager();

    for (auto entity : m_entities)
    {
        auto* sheet = entities.getComponent<entity::CSpriteSheet>(entity, Component::SpriteSheet);
        sheet->getSpriteSheet()->update(l_dt);

        // TODO: handle dispatching of death and attack messages
    }
}

void SSheetAnimation::notify(const messaging::Message& l_message)
{
    using namespace messaging;

    if (!hasEntity(l_message.m_receiver))
    {
        return;
    }

    const auto MessageType = (EntityMessage)l_message.m_type;

    switch (MessageType)
    {
    case EntityMessage::State_Changed:
    {
        using namespace entity;

        const auto NewEntityState = (EntityState)l_message.m_int;
        ASSERT(l_message.m_receiver >= 0, "");

        const auto Entity = (EntityId)l_message.m_receiver;

        switch (NewEntityState)
        {
        case EntityState::Idle:
        {
            changeAnimation(Entity, "Idle", true, true);
            break;
        }
        case EntityState::Walking:
        {
            changeAnimation(Entity, "Walk", true, true);
            break;
        }
        case EntityState::Running:
        {
            changeAnimation(Entity, "Run", true, true);
            break;
        }
        case EntityState::Attacking:
        {
            changeAnimation(Entity, "Attack", true, false);
            break;
        }
        case EntityState::Jumping:
        {
            changeAnimation(Entity, "Jump", true, false);
            break;
        }
        case EntityState::Falling:
        {
            changeAnimation(Entity, "Fall", true, true);
            break;
        }
        case EntityState::Landing:
        {
            changeAnimation(Entity, "Land", true, false);
            break;
        }
        case EntityState::Hurt:
        {
            changeAnimation(Entity, "Hurt", true, false);
            break;
        }
        case EntityState::Dying:
        {
            changeAnimation(Entity, "Dead", true, false);
            break;
        }
        case EntityState::Count:
        {
            break;
        }
        }

        break;
    }
    default:
        break;
    }
}

void SSheetAnimation::handleEvent(EntityId /*l_entity*/, messaging::EntityEvent /*l_event*/) {}

void SSheetAnimation::changeAnimation(EntityId l_entity, const std::string& l_anim, bool l_play,
                                      bool l_loop)
{
    auto* sheet = m_systemManager.getEntityManager().getComponent<entity::CSpriteSheet>(
        l_entity, Component::SpriteSheet);
    sheet->getSpriteSheet()->setAnimation(l_anim, l_play, l_loop);
}

} // namespace ecs::system