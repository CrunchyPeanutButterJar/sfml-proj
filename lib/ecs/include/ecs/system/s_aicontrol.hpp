#ifndef ECS_SYSTEM_S_AICONTROL_HPP
#define ECS_SYSTEM_S_AICONTROL_HPP

#include "core/directions.hpp"
#include <ecs/system/s_base.hpp>

namespace ecs::system
{
class SAIControl : public SBase
{
  public:
    SAIControl(SystemManager& l_systemManager);

    void update(float l_dt) override;
    void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) override;
    void notify(const messaging::Message& l_message) override;
};
} // namespace ecs::system

#endif