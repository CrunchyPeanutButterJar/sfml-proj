#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <SFML/Graphics.hpp>

#include <eventmanager.hpp>

class Window
{
public:

    Window();
    Window(const std::string& l_title,const sf::Vector2u& l_size);
    ~Window();
    void BeginDraw(); // Clear the window.
    void EndDraw(); // Display the changes.
    void Update();
    inline void SetAsDone() { m_isDone = true; }
    bool IsDone();
    bool IsFullscreen();
    sf::Vector2u GetWindowSize();
    void ToggleFullscreen();
    void Draw(sf::Drawable& l_drawable);
    sf::RenderWindow* GetRenderWindow();

    inline EventManager* GetEventManager()
    {
        return &m_eventManager;
    }

private:
    void Setup(const std::string& l_title, const sf::Vector2u& l_size);
    void Destroy();
    void Create();

private:
    EventManager m_eventManager;
    sf::RenderWindow m_window;
    sf::Vector2u m_windowSize;
    std::string m_windowTitle;
    bool m_isDone;
    bool m_isFullscreen; 

};

#endif