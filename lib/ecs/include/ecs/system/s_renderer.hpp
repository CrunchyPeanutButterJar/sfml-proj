#ifndef ECS_SYSTEM_S_RENDERER_HPP
#define ECS_SYSTEM_S_RENDERER_HPP

#include <ecs/system/s_base.hpp>
#include <core/directions.hpp>
#include <core/window.hpp>

class SRenderer : public SBase
{
public:
    SRenderer(SystemManager& l_systemManager);

    void update(float l_dt) override;
    void handleEvent(EntityId l_entity, EntityEvent l_event) override;
    void notify(const Message& l_message) override;
    void render(Window& l_window);

private:
    void setSheetDirection(EntityId l_entity, Direction l_dir);
};

#endif