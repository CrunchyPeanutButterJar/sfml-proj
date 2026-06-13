#include <gamestate.hpp>

#include <eventmanager.hpp>
#include <snake.hpp>
#include <statemanager.hpp>
#include <window.hpp>

#include <SFML/Window/Keyboard.hpp>

#include <functional>

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

GameState::GameState(StateManager& l_stateManager): BaseState(l_stateManager), m_world(m_stateManager.GetContext().m_window.GetWindowSize(), m_textbox), m_snake(m_world.GetBlockSize())
{
    m_textbox.Setup(5, 14, 350, sf::Vector2f(m_stateManager.GetContext().m_window.GetWindowSize().x/2 - 175, 0));
    time_t seed = time(nullptr);
    srand(static_cast<unsigned int>(seed));

    m_textbox.Add("Seeded random number generator with: " + std::to_string(seed));

    auto& [_, eventManager] = m_stateManager.GetContext();

    eventManager.AddCallback(StateType::Game, "Key_Escape", [&](const Events&){m_stateManager.SwitchTo(StateType::Paused);});

    eventManager.AddCallback(StateType::Game, "Game_MoveUp", std::bind(moveSnake<sf::Keyboard::Up>, std::ref(m_snake)));
    eventManager.AddCallback(StateType::Game, "Game_MoveDown", std::bind(moveSnake<sf::Keyboard::Down>, std::ref(m_snake)));
    eventManager.AddCallback(StateType::Game, "Game_MoveRight", std::bind(moveSnake<sf::Keyboard::Right>, std::ref(m_snake)));
    eventManager.AddCallback(StateType::Game, "Game_MoveLeft", std::bind(moveSnake<sf::Keyboard::Left>, std::ref(m_snake)));
}

GameState::~GameState()
{
    auto& [_, eventManager] = m_stateManager.GetContext();

    eventManager.RemoveCallback(StateType::Game, "Key_Escape");

    eventManager.RemoveCallback(StateType::Game, "Game_MoveUp");
    eventManager.RemoveCallback(StateType::Game, "Game_MoveDown");
    eventManager.RemoveCallback(StateType::Game, "Game_MoveRight");
    eventManager.RemoveCallback(StateType::Game, "Game_MoveLeft");
}

void GameState::Update(const sf::Time& l_elapsed)
{
    m_elapsed += l_elapsed;

    float timestep = 1.0f / m_snake.GetSpeed();

    if(m_elapsed.asSeconds() >= timestep)
    {
        m_snake.Tick();
        m_world.Update(m_snake);
        if(m_snake.HasLost())
        {
            m_textbox.Add("GAME OVER! Your score: " + std::to_string(m_snake.GetScore()));
            m_snake.Reset();
        }

        m_elapsed -= sf::seconds(timestep);
    }
}

void GameState::Draw()
{
    auto& [window, _] = m_stateManager.GetContext();

    m_world.Render(*window.GetRenderWindow());
    m_snake.Render(*window.GetRenderWindow());
    m_textbox.Render(*window.GetRenderWindow());
}

void GameState::Activate()
{
}

void GameState::Deactivate()
{
}

void GameState::OnCreate()
{
}