#ifndef ECS_MESSAGING_ENTITY_EVENTS_HPP
#define ECS_MESSAGING_ENTITY_EVENTS_HPP

namespace ecs::messaging
{
enum class EntityEvent
{
    Spawned,
    Despawned,
    Colliding_X,
    Colliding_Y,
    Moving_Left,
    Moving_Right,
    Jump_Attempt,
    Jumped,
    Falling,
    Not_Grounded,
    Became_Idle,
    Began_Moving
};
} // namespace ecs::messaging
#endif