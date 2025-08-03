#include <window.hpp>

#include <functional>

Window::Window()
{
    Setup("Window", sf::Vector2u(640, 480));
}

Window::Window(const std::string& l_title, const sf::Vector2u& l_size)
{
    Setup(l_title, l_size);
}

Window::~Window()
{
    Destroy();
}

void Window::Setup(const std::string& l_title, const sf::Vector2u& l_size)
{
    m_windowTitle = l_title;
    m_windowSize = l_size;
    m_isDone = false;
    m_isFullscreen = false;

    m_eventManager.AddCallback(StateType{0}, "Window_Close", std::bind(&Window::SetAsDone, std::ref(*this)));
    m_eventManager.AddCallback(StateType{0}, "Window_ToggleFullscreen", std::bind(&Window::ToggleFullscreen, std::ref(*this)));

    Create();
}

void Window::Create()
{
    auto style = (m_isFullscreen ? sf::Style::Fullscreen : sf::Style::Default);
    m_window.create({m_windowSize.x, m_windowSize.y, 32}, m_windowTitle, style);
}

void Window::Destroy()
{
    m_window.close();
}

sf::RenderWindow* Window::GetRenderWindow()
{
    return &m_window;
}

EventManager& Window::GetEventManager()
{
    return m_eventManager;
}

sf::FloatRect Window::GetViewSpace() const
{
    const sf::Vector2f viewCenter = m_window.getView().getCenter();
    const sf::Vector2f viewSize = m_window.getView().getSize();
    const sf::Vector2f viewSizeHalf{viewSize.x / 2, viewSize.y / 2};

    return {viewCenter - viewSizeHalf, viewSize};
}


void Window::Update(StateType l_state)
{
    sf::Event event;

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
            m_eventManager.HandleEvent(event);
        }
    }
    
    m_eventManager.Update(l_state);
}

void Window::ToggleFullscreen()
{
    m_isFullscreen = !m_isFullscreen;
    Destroy();
    Create();
}

void Window::BeginDraw()
{
    m_window.clear(sf::Color::Black);
}

void Window::EndDraw()
{
    m_window.display();
}

bool Window::IsDone()
{
    return m_isDone;
}

bool Window::IsFullscreen()
{
    return m_isFullscreen;
}

sf::Vector2u Window::GetWindowSize()
{
    return m_windowSize;
}

void Window::Draw(sf::Drawable& l_drawable)
{
    m_window.draw(l_drawable);
}