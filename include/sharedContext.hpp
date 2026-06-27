#ifndef SHAREDCONTEXT_HPP
#define SHAREDCONTEXT_HPP

#include <textureManager.hpp>
#include <eventmanager.fwd.hpp>
#include <window.fwd.hpp>

struct SharedContext
{
    Window& m_window;
    EventManager& m_eventManager;
    TextureManager m_textureManager;
};

#endif