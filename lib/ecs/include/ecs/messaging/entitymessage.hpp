#ifndef ECS_MESSAGING_ENTITYMESSAGE_HPP
#define ECS_MESSAGING_ENTITYMESSAGE_HPP

namespace ecs::messaging
{
enum class EntityMessage
{
    Move, Is_Moving, State_Changed, Direction_Changed,
    Switch_State, Attack_Action, Dead
};
}

#endif