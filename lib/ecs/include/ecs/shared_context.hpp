#ifndef ECS_SHARED_CONTEXT_HPP
#define ECS_SHARED_CONTEXT_HPP

#include <core/event_manager.fwd.hpp>
#include <core/graphics/texture_manager.fwd.hpp>
#include <core/window.fwd.hpp>
#include <ecs/entity/entity_manager.fwd.hpp>
#include <ecs/system/system_manager.fwd.hpp>

namespace ecs
{
struct SharedContext
{
    core::Window&                   m_window;
    core::EventManager&             m_eventManager;
    entity::EntityManager&          m_entityManager;
    system::SystemManager&          m_systemManager;
    core::graphics::TextureManager& m_textureManager;
};
} // namespace ecs

#endif