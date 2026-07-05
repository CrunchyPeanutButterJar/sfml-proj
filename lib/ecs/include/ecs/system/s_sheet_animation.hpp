#ifndef ECS_SYSTEM_S_SHEET_ANIMATION_HPP
#define ECS_SYSTEM_S_SHEET_ANIMATION_HPP

#include <ecs/system/s_base.hpp>

namespace ecs::system
{
class SSheetAnimation : public SBase
{
  public:
    SSheetAnimation(SystemManager& l_systemManager);

    void update(float l_dt) override;
    void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) override;
    void notify(const messaging::Message& l_message) override;

  private:
    void changeAnimation(EntityId l_entity, const std::string& l_anim, bool l_play, bool l_loop);
};
} // namespace ecs::system

#endif