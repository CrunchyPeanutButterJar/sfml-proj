#ifndef CORE_WINDOW_HPP
#define CORE_WINDOW_HPP

#include <SFML/Graphics.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <core/eventmanager.hpp>

class Window
{
public:

    Window();
    Window(const std::string& l_title, const sf::Vector2u& l_size);
    ~Window();
    void beginDraw(); // Clear the window.
    void endDraw(); // Display the changes.
    void update(EventManager::StateType l_state);
    void setAsDone() { m_isDone = true; }
    bool isDone() const;
    bool isFullscreen() const;
    sf::Vector2u getWindowSize();
    void toggleFullscreen();
    void draw(sf::Drawable& l_drawable);
    sf::RenderWindow* getRenderWindow();
    EventManager& getEventManager();
    sf::FloatRect getViewSpace() const;

private:
    void setup(const std::string& l_title, const sf::Vector2u& l_size);
    void destroy();
    void create();


    EventManager m_eventManager;
    sf::RenderWindow m_window;
    sf::Vector2u m_windowSize;
    std::string m_windowTitle;
    bool m_isDone{};
    bool m_isFullscreen{};
    bool m_isFocused{}; 

};

#endif