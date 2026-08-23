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
    Moving_Right,
    Moving_Left,
    Moving_Up,
    Moving_down,
    Jump_Attempt,
    Jumped,
    Falling,
    Not_Grounded,
    Became_Idle,
    Is_Moving,
    Began_Moving,
    Animation_Done
};
} // namespace ecs::messaging
#endif