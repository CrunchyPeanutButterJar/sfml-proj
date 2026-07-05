#include <core/window.hpp>

#include <functional>

using namespace core;

Window::Window()
{
    setup("Window", sf::Vector2u(640, 480));
}

Window::Window(const std::string& l_title, const sf::Vector2u& l_size)
{
    setup(l_title, l_size);
}

Window::~Window()
{
    destroy();
}

void Window::setup(const std::string& l_title, const sf::Vector2u& l_size)
{
    m_windowTitle = l_title;
    m_windowSize = l_size;
    m_isDone = false;
    m_isFullscreen = false;

    m_eventManager.addCallback(0, "Window_Close", std::bind(&Window::setAsDone, std::ref(*this)));
    m_eventManager.addCallback(0, "Window_ToggleFullscreen", std::bind(&Window::toggleFullscreen, std::ref(*this)));

    create();
}

void Window::create()
{
    auto style = (m_isFullscreen ? sf::Style::Fullscreen : sf::Style::Default);
    m_window.create({m_windowSize.x, m_windowSize.y, 32}, m_windowTitle, style);
}

void Window::destroy()
{
    m_window.close();
}

sf::RenderWindow* Window::getRenderWindow()
{
    return &m_window;
}

core::EventManager& Window::getEventManager()
{
    return m_eventManager;
}

sf::FloatRect Window::getViewSpace() const
{
    const sf::Vector2f ViewCenter = m_window.getView().getCenter();
    const sf::Vector2f ViewSize = m_window.getView().getSize();
    const sf::Vector2f ViewSizeHalf{ViewSize.x / 2, ViewSize.y / 2};

    return {ViewCenter - ViewSizeHalf, ViewSize};
}


void Window::update(core::EventManager::StateType l_state)
{
    sf::Event event{};

    while(m_window.pollEvent(event))
    {
        if(event.type == sf::Event::LostFocus)
        {
            m_isFocused = false;
        }
        else if(event.type == sf::Event::GainedFocus)
        {
            m_isFocused = true;
        }

        if(m_isFocused)
        {
            m_eventManager.handleEvent(event);
        }
    }

    if(m_isFocused)
    {
        m_eventManager.update(l_state, m_window);
    }
}

void Window::toggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;
    destroy();
    create();
}

void Window::beginDraw()
{
    m_window.clear(sf::Color::Black);
}

void Window::endDraw()
{
    m_window.display();
}

bool Window::isDone() const
{
    return m_isDone;
}

bool Window::isFullscreen() const
{
    return m_isFullscreen;
}

sf::Vector2u Window::getWindowSize()
{
    return m_windowSize;
}

void Window::draw(sf::Drawable& l_drawable)
{
    m_window.draw(l_drawable);
}