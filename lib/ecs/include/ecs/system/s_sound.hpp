#ifndef ECS_SYSTEM_S_SOUND_HPP
#define ECS_SYSTEM_S_SOUND_HPP

#include <SFML/System/Vector3.hpp>
#include <core/audio/audio_manager.fwd.hpp>
#include <core/audio/sound_manager.fwd.hpp>
#include <ecs/entity/c_sound_emitter.fwd.hpp>
#include <ecs/system/s_base.hpp>

namespace ecs::system
{
class SSound : public SBase
{
  public:
    SSound(SystemManager& l_systemMgr);
    ~SSound() override;

    void update(float l_dT) override;
    void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) override;
    void notify(const messaging::Message& l_message) override;

    void setUp(core::audio::AudioManager* l_audioManager,
               core::audio::SoundManager* l_soundManager);

  private:
    static auto makeSoundPosition(const sf::Vector2f& l_entityPos) -> sf::Vector3f;
    void        emitSound(EntityId l_entity, ecs::entity::EntitySound l_sound, bool l_useId,
                          bool l_relative, int l_checkFrame = -1);

    core::audio::AudioManager* m_audioManager{};
    core::audio::SoundManager* m_soundManager{};
};
} // namespace ecs::system
#endif
