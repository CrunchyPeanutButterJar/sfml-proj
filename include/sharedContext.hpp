#ifndef SHAREDCONTEXT_HPP
#define SHAREDCONTEXT_HPP

#include <core/graphics/texture_manager.fwd.hpp>
#include <core/event_manager.fwd.hpp>
#include <ecs/entity/entity_manager.fwd.hpp>
#include <ecs/system/system_manager.fwd.hpp>
#include <core/window.fwd.hpp>

struct SharedContext
{
    core::Window& m_window;
    core::EventManager& m_eventManager;
    ecs::entity::EntityManager& m_entityManager;
    ecs::system::SystemManager& m_systemManager;
    core::graphics::TextureManager& m_textureManager;
};

#endif