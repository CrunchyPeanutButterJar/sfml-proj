#ifndef ECS_SYSTEM_S_MOVEMENT_HPP
#define ECS_SYSTEM_S_MOVEMENT_HPP

#include <SFML/System/Vector2.hpp>
#include <core/directions.hpp>
#include <core/graphics/tiles.fwd.hpp>
#include <ecs/entity/c_movable.fwd.hpp>
#include <ecs/entity/c_position.fwd.hpp>
#include <ecs/map.fwd.hpp>
#include <ecs/system/s_base.hpp>

namespace ecs::system
{
enum class Axis : std::uint8_t
{
    x,
    y
};

class SMovement : public SBase
{
  public:
    SMovement(SystemManager& l_systemManager);

    void setMap(Map* l_map);

    void update(float l_dt) override;
    void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) override;
    void notify(const messaging::Message& l_message) override;

  private:
    static void movementStep(float l_dt, entity::CMovable* l_movable,
                             const core::graphics::Tile* l_tile, float l_gravity);
    void        setDirection(EntityId l_entity, core::Direction l_dir);
    void        stopEntity(EntityId l_entity, Axis l_axis);

    Map* m_map{};
};
} // namespace ecs::system

#endif