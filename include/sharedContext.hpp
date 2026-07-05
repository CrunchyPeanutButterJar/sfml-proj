#ifndef SHAREDCONTEXT_HPP
#define SHAREDCONTEXT_HPP

#include <core/graphics/textureManager.fwd.hpp>
#include <core/eventmanager.fwd.hpp>
#include <ecs/entity/entitymanager.fwd.hpp>
#include <ecs/system/systemmanager.fwd.hpp>
#include <core/window.fwd.hpp>

struct SharedContext
{
    core::Window& m_window;
    core::EventManager& m_eventManager;
    EntityManager& m_entityManager;
    SystemManager& m_systemManager;
    core::graphics::TextureManager& m_textureManager;
};

#endif