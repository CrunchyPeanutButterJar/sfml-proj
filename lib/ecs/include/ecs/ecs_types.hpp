#ifndef ECS_ECS_TYPES_HPP
#define ECS_ECS_TYPES_HPP

namespace ecs
{
using EntityId = unsigned int;

enum
{
    N_COMPONENT_TYPES = 32
};
using ComponentType = unsigned int;

enum class Component
{
    Position = 0,
    SpriteSheet,
    State,
    Movable,
    Controller,
    Collidable,
    SoundEmitter,
    SoundListener,
    AIController
};

enum class System
{
    Renderer = 0,
    AIControl,
    Control,
    Movement,
    Collision,
    State,
    SheetAnimation,
    Sound
};
} // namespace ecs

#endif