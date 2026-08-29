#ifndef ECS_SYSTEM_S_CONTROL_HPP
#define ECS_SYSTEM_S_CONTROL_HPP

#include "core/directions.hpp"
#include <ecs/ecs_types.hpp>
#include <ecs/messaging/message_handler.fwd.hpp>
#include <ecs/messaging/message_handler.hpp>
#include <ecs/system/s_base.hpp>

namespace ecs::system
{
class SControl : public SBase
{
  public:
    SControl(SystemManager& l_systemManager);

    void update(float l_dt) override;
    void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) override;
    void notify(const messaging::Message& l_message) override;

  private:
    void moveEntity(EntityId l_entity, core::Direction l_dir);
};

void moveEntity(ecs::messaging::MessageHandler& l_messageHandler, ecs::EntityId l_entity,
                core::Direction l_dir);

void entityAttack(ecs::messaging::MessageHandler& l_messageHandler, ecs::EntityId l_entity);

} // namespace ecs::system

#endif