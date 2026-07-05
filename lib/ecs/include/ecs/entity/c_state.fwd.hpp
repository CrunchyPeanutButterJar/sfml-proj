#ifndef ECS_ENTITY_C_STATE_FWD_HPP
#define ECS_ENTITY_C_STATE_FWD_HPP

#include <cstdint>

namespace ecs::entity
{
enum class EntityState : std::uint8_t
{
    Idle,
    Walking,
    Running,
    Attacking,
    Hurt,
    Dying
};

class CState;
} // namespace ecs::entity

#endif