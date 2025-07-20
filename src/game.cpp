#include <game.hpp>


template<sf::Keyboard::Key direction>
static void moveSnake(Snake& l_snake) 
{
    if constexpr (direction == sf::Keyboard::Up)
    {
        if(l_snake.GetPhysicalDirection() != Direction::Down)
        {
            l_snake.SetDirection(Direction::Up);
        }
    }
    else if constexpr (direction == sf::Keyboard::Down)
    {
        if(l_snake.GetPhysicalDirection() != Direction::Up)
        {
            l_snake.SetDirection(Direction::Down);
        }
    }
    else if constexpr (direction == sf::Keyboard::Left)
    {
        if(l_snake.GetPhysicalDirection() != Direction::Right)
        {
            l_snake.SetDirection(Direction::Left);
        }
    }
    else if constexpr (direction == sf::Keyboard::Right)
    {
        if(l_snake.GetPhysicalDirection() != Direction::Left)
        {
            l_snake.SetDirection(Direction::Right);
        }
    }
}

static EventManager createEventManager(Snake& l_snake)
{
    EventManager eventManager;

    eventManager.AddCallback("MoveUp", std::bind(moveSnake<sf::Keyboard::Up>, std::ref(l_snake)));
    eventManager.AddCallback("MoveDown", std::bind(moveSnake<sf::Keyboard::Down>, std::ref(l_snake)));
    eventManager.AddCallback("MoveRight", std::bind(moveSnake<sf::Keyboard::Right>, std::ref(l_snake)));
    eventManager.AddCallback("MoveLeft", std::bind(moveSnake<sf::Keyboard::Left>, std::ref(l_snake)));

    return eventManager;
}

Game::Game() : m_window{"snake", sf::Vector2u(800, 600), createEventManager(m_snake)}, m_world(sf::Vector2u(800, 600), m_textbox), m_snake(m_world.GetBlockSize())
{
    m_textbox.Setup(5, 14, 350, sf::Vector2f(225, 0));
    m_textbox.Add("Seeded random number generator with: " + std::to_string(time(nullptr)));
}

void Game::Update()
{
    m_window.Update();

    float timestep = 1.0f / m_snake.GetSpeed();

    if(m_elapsed.asSeconds() >= timestep)
    {
        m_snake.Tick();
        m_world.Update(m_snake);
        m_elapsed -= sf::seconds(timestep);
        if(m_snake.HasLost())
        {
            m_textbox.Add("GAME OVER! Your score: " + std::to_string(m_snake.GetScore()));
            m_snake.Reset();
        }
    }
}

sf::Time Game::GetElapsed()
{
    return m_elapsed;
}

void Game::RestartClock()
{
    m_elapsed += m_clock.restart();
}

Window* Game::GetWindow()
{
    return &m_window;
}

void Game::Render()
{
    m_window.BeginDraw();

    m_world.Render(*m_window.GetRenderWindow());
    m_snake.Render(*m_window.GetRenderWindow());
    m_textbox.Render(*m_window.GetRenderWindow());

    m_window.EndDraw();
}