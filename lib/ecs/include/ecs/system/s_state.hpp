#ifndef ECS_SYSTEM_S_STATE_HPP
#define ECS_SYSTEM_S_STATE_HPP

#include <ecs/entity/c_state.fwd.hpp>
#include <ecs/system/s_base.hpp>

namespace ecs::system
{
class SState : public SBase
{
  public:
    SState(SystemManager& l_systemManager);

    void update(float l_dt) override;
    void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) override;
    void notify(const messaging::Message& l_message) override;

  private:
    void changeState(EntityId l_entity, entity::EntityState l_state, bool l_force);
};
} // namespace ecs::system

#endif