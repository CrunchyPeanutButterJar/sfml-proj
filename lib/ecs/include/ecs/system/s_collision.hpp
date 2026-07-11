#ifndef ECS_SYSTEM_S_COLLISION_HPP
#define ECS_SYSTEM_S_COLLISION_HPP

#include "ecs/entity/c_position.hpp"
#include "ecs/map.hpp"
#include <ecs/entity/c_collidable.fwd.hpp>
#include <ecs/entity/c_position.fwd.hpp>
#include <ecs/map.fwd.hpp>
#include <ecs/system/s_base.hpp>

namespace ecs::system
{
class SCollision : public SBase
{
  public:
    SCollision(SystemManager& l_systemManager);

    void setMap(ecs::Map* l_map);

    void update(float l_dt) override;
    void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) override;
    void notify(const messaging::Message& l_message) override;

  private:
    void checkOutOfBounds(entity::CPosition* position, entity::CCollidable* collidable);
    void mapCollisions(EntityId l_entity, entity::CPosition* l_position,
                       entity::CCollidable* l_collidable);

    ecs::Map* m_map{nullptr};
};
} // namespace ecs::system

#endif