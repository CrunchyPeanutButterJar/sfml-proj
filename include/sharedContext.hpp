#ifndef SHAREDCONTEXT_HPP
#define SHAREDCONTEXT_HPP

#include <core/graphics/textureManager.fwd.hpp>
#include <core/eventmanager.fwd.hpp>
#include <ecs/entity/entitymanager.fwd.hpp>
#include <ecs/system/systemmanager.fwd.hpp>
#include <core/window.fwd.hpp>

struct SharedContext
{
    Window& m_window;
    EventManager& m_eventManager;
    EntityManager& m_entityManager;
    SystemManager& m_systemManager;
    TextureManager& m_textureManager;
};

#endif