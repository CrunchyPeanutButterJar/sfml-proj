#ifndef ECS_ENTITY_C_SOUND_EMITTER_FWD_HPP
#define ECS_ENTITY_C_SOUND_EMITTER_FWD_HPP

#include <core/audio/sound_manager.fwd.hpp>
#include <cstdint>

namespace ecs::entity
{

using SoundID = core::audio::SoundID;

enum class EntitySound : std::uint8_t
{
    Footstep,
    Jump,
    Attack,
    WingsFlap,
    Hurt,
    Death,
    Last
};

struct SoundParameters;
class CSoundEmitter;
} // namespace ecs::entity
#endif
