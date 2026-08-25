#ifndef ECS_SYSTEM_S_COLLISION_HPP
#define ECS_SYSTEM_S_COLLISION_HPP

#include "ecs/entity/c_collidable.hpp"
#include "ecs/entity/c_position.hpp"
#include "ecs/map.hpp"
#include <ecs/ecs_types.hpp>
#include <ecs/entity/c_collidable.fwd.hpp>
#include <ecs/entity/c_position.fwd.hpp>
#include <ecs/map.fwd.hpp>
#include <ecs/shared_context.hpp>
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
    void lateUpdate() override;

  private:
    void checkOutOfBounds(entity::CPosition* position, entity::CCollidable* collidable);
    void mapCollisions(EntityId l_entity, entity::CPosition* l_position,
                       entity::CCollidable* l_collidable);

    ecs::Map* m_map{nullptr};
    std::vector<std::tuple<EntityId, EntityId, entity::EntityTag, entity::EntityTag>>
        m_entityCollisions;
};

using CollisionResolutionFun = void (*)(ecs::EntityId, ecs::EntityId, ecs::SharedContext*);
void registerCollisionResolution(entity::EntityTag l_entity1, entity::EntityTag l_entity2,
                                 CollisionResolutionFun fun);
} // namespace ecs::system

#endif