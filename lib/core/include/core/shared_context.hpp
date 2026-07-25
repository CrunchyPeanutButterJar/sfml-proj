#ifndef CORE_SHARED_CONTEXT_HPP
#define CORE_SHARED_CONTEXT_HPP

#include <core/event_manager.fwd.hpp>
#include <core/font_manager.fwd.hpp>
#include <core/graphics/texture_manager.fwd.hpp>
#include <core/gui/GUI_manager.fwd.hpp>
#include <core/window.fwd.hpp>

namespace core
{
struct SharedContext
{
    Window&                   m_window;
    EventManager&             m_eventManager;
    graphics::TextureManager& m_textureManager;
    FontManager&              m_fontManager;
    gui::GUI_Manager&         m_guiManager;
};
} // namespace core

#endif