#ifndef ECS_SYSTEM_S_RENDERER_HPP
#define ECS_SYSTEM_S_RENDERER_HPP

#include <ecs/system/s_base.hpp>
#include <core/directions.hpp>
#include <core/window.hpp>

namespace ecs::system
{
class SRenderer : public SBase
{
public:
    SRenderer(SystemManager& l_systemManager);

    void update(float l_dt) override;
    void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) override;
    void notify(const messaging::Message& l_message) override;
    void render(core::Window& l_window);

private:
    void setSheetDirection(EntityId l_entity, core::Direction l_dir);
};
} // namespace ecs::system

#endif