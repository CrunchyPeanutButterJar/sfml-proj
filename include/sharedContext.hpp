#ifndef SHAREDCONTEXT_HPP
#define SHAREDCONTEXT_HPP

#include <textureManager.fwd.hpp>
#include <eventmanager.fwd.hpp>
#include <entitymanager.fwd.hpp>
#include <systemmanager.fwd.hpp>
#include <window.fwd.hpp>

struct SharedContext
{
    Window& m_window;
    EventManager& m_eventManager;
    EntityManager& m_entityManager;
    SystemManager& m_systemManager;
    TextureManager& m_textureManager;
};

#endif