#ifndef CORE_WINDOW_HPP
#define CORE_WINDOW_HPP

#include <SFML/Graphics.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/event_manager.hpp>

namespace core
{
class Window
{
  public:
    Window();
    Window(const std::string& l_title, const sf::Vector2u& l_actualSize,
           const sf::Vector2u& l_scaledSize);
    ~Window();
    void beginDraw(); // Clear the window.
    void endDraw();   // Display the changes.
    void update(core::state::StateType l_state);
    void setAsDone() { m_isDone = true; }
    auto isDone() const -> bool;
    auto isFullscreen() const -> bool;
    auto getWindowSize() -> sf::Vector2u;
    void toggleFullscreen();
    void draw(sf::Drawable& l_drawable);
    auto getRenderWindow() -> sf::RenderWindow*;
    auto getEventManager() -> core::EventManager&;
    auto getViewSpace() const -> sf::FloatRect;
    auto getMousePosition() const -> sf::Vector2f;

  private:
    void setup(const std::string& l_title, const sf::Vector2u& l_size);
    void destroy();
    void create();

    core::EventManager m_eventManager;
    sf::RenderWindow   m_window;
    sf::Vector2u       m_windowSize;
    std::string        m_windowTitle;
    bool               m_isDone{};
    bool               m_isFullscreen{};
    bool               m_isFocused{};
};

auto getOriginalPoint(sf::Vector2f l_point, Window& l_window) -> sf::Vector2f;

} // namespace core

#endif