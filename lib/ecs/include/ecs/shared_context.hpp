#ifndef ECS_SHARED_CONTEXT_HPP
#define ECS_SHARED_CONTEXT_HPP

#include <core/shared_context.hpp>
#include <ecs/entity/entity_manager.fwd.hpp>
#include <ecs/system/system_manager.fwd.hpp>
#include <memory>
namespace ecs
{

struct SharedContext : public core::SharedContext
{
    SharedContext(core::SharedContext l_context, entity::EntityManager& l_entityManager,
                  system::SystemManager& l_systemManager)
        : core::SharedContext{l_context}, m_entityManager{l_entityManager},
          m_systemManager{l_systemManager}
    {
    }

    entity::EntityManager& m_entityManager;
    system::SystemManager& m_systemManager;
};

struct SharedContextBuilder
{
    core::Window&                   m_window;
    core::EventManager&             m_eventManager;
    core::graphics::TextureManager& m_textureManager;
    entity::EntityManager&          m_entityManager;
    system::SystemManager&          m_systemManager;

    static auto build(SharedContextBuilder l_builder) -> std::unique_ptr<SharedContext>
    {
        return std::make_unique<SharedContext>(
            core::SharedContext{l_builder.m_window, l_builder.m_eventManager,
                                l_builder.m_textureManager},
            l_builder.m_entityManager, l_builder.m_systemManager);
    }
};

} // namespace ecs

#endif