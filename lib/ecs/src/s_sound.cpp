#include "ecs/entity/c_sound_emitter.fwd.hpp"
#include "ecs/entity/c_state.hpp"
#include <SFML/Audio/Listener.hpp>
#include <core/audio/audio_manager.hpp>
#include <core/audio/sound_manager.hpp>
#include <core/directions.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/c_sound_emitter.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/map.hpp>
#include <ecs/messaging/message_handler.hpp>
#include <ecs/system/s_sound.hpp>
#include <ecs/system/system_manager.hpp>
#include <utils/bitmask.hpp>

using namespace ecs::system;

SSound::SSound(SystemManager& l_systemMgr) : SBase(System::Sound, l_systemMgr)
{
    utils::Bitmask req;
    req.turnOnBit((unsigned int)Component::Position);
    req.turnOnBit((unsigned int)Component::SoundEmitter);
    m_requiredComponents.push_back(req);

    req.clearBit((unsigned int)Component::SoundEmitter);
    req.turnOnBit((unsigned int)Component::SoundListener);
    m_requiredComponents.push_back(req);

    m_systemManager.getMessageHandler().subscribe(messaging::EntityMessage::Direction_Changed,
                                                  this);
    m_systemManager.getMessageHandler().subscribe(messaging::EntityMessage::Frame_Change, this);
}

SSound::~SSound() = default;

void SSound::update(float /*l_dT*/)
{
    auto& entities = m_systemManager.getEntityManager();
    for (auto& entity : m_entities)
    {
        auto*       c_pos = entities.getComponent<entity::CPosition>(entity, Component::Position);
        const auto& position = c_pos->getPosition();

        const bool IsListener = entities.hasComponent(entity, Component::SoundListener);
        if (IsListener)
        {
            sf::Listener::setPosition(makeSoundPosition(position));
        }

        if (!entities.hasComponent(entity, Component::SoundEmitter))
        {
            continue;
        }
        auto* c_snd = entities.getComponent<entity::CSoundEmitter>(entity, Component::SoundEmitter);
        auto  sound_id = c_snd->getSoundId();
        if (!sound_id.has_value())
        {
            continue;
        }
        if (!IsListener)
        {
            if (!m_soundManager->setPosition(*sound_id, makeSoundPosition(position)))
            {
                c_snd->setSoundId({});
            }
        }
        else
        {
            if (!m_soundManager->isPlaying(*sound_id))
            {
                c_snd->setSoundId({});
            }
        }
    }
}

void SSound::handleEvent(EntityId /*l_entity*/, messaging::EntityEvent /*l_event*/) {}

void SSound::notify(const messaging::Message& l_message)
{
    if (!hasEntity(l_message.m_receiver))
    {
        return;
    }
    auto&      entities   = m_systemManager.getEntityManager();
    const bool IsListener = entities.hasComponent(l_message.m_receiver, Component::SoundListener);

    auto m = (messaging::EntityMessage)l_message.m_type;
    switch (m)
    {
    case messaging::EntityMessage::Direction_Changed:
    {
        if (!IsListener)
        {
            return;
        }
        auto dir = (core::Direction)l_message.m_int;
        switch (dir)
        {
        case core::Direction::Left:
            sf::Listener::setDirection(-1, 0, 0);
            break;
        case core::Direction::Right:
            sf::Listener::setDirection(1, 0, 0);
            break;
        }
        break;
    }
    case messaging::EntityMessage::Frame_Change:
    {
        using namespace ecs::entity;

        if (!entities.hasComponent(l_message.m_receiver, Component::SoundEmitter))
        {
            return;
        }
        auto state = entities.getComponent<entity::CState>(l_message.m_receiver, Component::State)
                         ->getState();
        std::optional<EntitySound> sound;

        if (state == EntityState::Running)
        {
            sound = EntitySound::Footstep;
        }
        else if (state == EntityState::Jumping)
        {
            sound = EntitySound::Jump;
        }
        else if (state == EntityState::Attacking)
        {
            sound = EntitySound::Attack;
        }
        else if (state == EntityState::Hurt)
        {
            sound = EntitySound::Hurt;
        }
        else if (state == EntityState::Dying)
        {
            sound = EntitySound::Death;
        }

        if (sound.has_value())
        {
            emitSound(l_message.m_receiver, sound.value(), false, IsListener, l_message.m_int);
        }
        break;
    }

    default:
        break;
    }
}

void SSound::setUp(core::audio::AudioManager* l_audioManager,
                   core::audio::SoundManager* l_soundManager)
{
    m_audioManager = l_audioManager;
    m_soundManager = l_soundManager;
}

auto SSound::makeSoundPosition(const sf::Vector2f& l_entityPos) -> sf::Vector3f
{
    return {l_entityPos.x, l_entityPos.y, 0};
}

void SSound::emitSound(EntityId l_entity, entity::EntitySound l_sound, bool l_useId,
                       bool l_relative, int l_checkFrame)
{
    if (!hasEntity(l_entity))
    {
        return;
    }
    if (!m_systemManager.getEntityManager().hasComponent(l_entity, Component::SoundEmitter))
    {
        return;
    }
    // Is a sound emitter.
    auto& entities = m_systemManager.getEntityManager();
    auto* c_snd = entities.getComponent<entity::CSoundEmitter>(l_entity, Component::SoundEmitter);

    auto sound_id = c_snd->getSoundId();

    if (sound_id.has_value() && l_useId)
    {
        return;
    } // Use of ID is required AND sound is occupied.
    // If sound is free or use of ID isn't required.
    if (l_checkFrame != -1 && !c_snd->isSoundFrame(l_sound, l_checkFrame))
    {
        return;
    }
    // Frame is irrelevant or correct.
    auto*        c_pos = entities.getComponent<entity::CPosition>(l_entity, Component::Position);
    sf::Vector3f pos =
        (l_relative ? sf::Vector3f(0.F, 0.F, 0.F) : makeSoundPosition(c_pos->getPosition()));
    if (l_useId)
    {
        c_snd->setSoundId(m_soundManager->play(c_snd->getSound(l_sound), pos));
    }
    else
    {
        m_soundManager->play(c_snd->getSound(l_sound), pos, false, l_relative);
    }
}