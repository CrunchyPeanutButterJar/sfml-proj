#ifndef ECS_ENTITY_C_SOUND_EMITTER_HPP
#define ECS_ENTITY_C_SOUND_EMITTER_HPP

#include <array>
#include <core/audio/sound_manager.fwd.hpp>
#include <ecs/entity/c_base.hpp>
#include <ecs/entity/c_sound_emitter.fwd.hpp>
#include <optional>
#include <string>
#include <utils/utilities.fwd.hpp>

namespace ecs::entity
{
struct SoundParameters
{
    static constexpr int MAX_SOUND_FRAMES = 5;

    SoundParameters() = default;

    std::string                                      m_sound;
    std::array<std::optional<int>, MAX_SOUND_FRAMES> m_frames{};
};

class CSoundEmitter : public CBase
{
  public:
    static constexpr int MAX_ENTITY_SOUNDS = (int)EntitySound::Last;

    CSoundEmitter();

    void readInput(utils::Tokens& l_tokens) override;

    auto getSoundId() -> std::optional<SoundID>;
    void setSoundId(std::optional<SoundID> l_id);

    auto getSound(EntitySound l_snd) -> std::string;

    auto isSoundFrame(EntitySound l_snd, int l_frame) -> bool;

  private:
    std::optional<SoundID>                         m_soundID;
    std::array<SoundParameters, MAX_ENTITY_SOUNDS> m_params;
};
} // namespace ecs::entity
#endif
