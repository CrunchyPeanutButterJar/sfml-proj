#ifndef ECS_MESSAGING_ENTITY_MESSAGE_HPP
#define ECS_MESSAGING_ENTITY_MESSAGE_HPP

namespace ecs::messaging
{
enum class EntityMessage
{
    Move,
    Jump,
    Is_Moving,
    State_Changed,
    Direction_Changed,
    Switch_State,
    Attack_Action,
    Dead,
    Frame_Change
};
} // namespace ecs::messaging

#endif