#include <game.hpp>

Game::Game() :
m_window{"snake", sf::Vector2u(800, 600)},
m_stateManager{std::make_tuple(std::ref(m_window), std::ref(m_window.GetEventManager()))}
{
    m_stateManager.SwitchTo(StateType::Game);
}

void Game::Update()
{
    m_window.Update(m_stateManager.GetCurrentState());
    m_stateManager.Update(m_elapsed);
}

void Game::LateUpdate()
{
    m_stateManager.ProcessRequests();
    RestartClock();
}

sf::Time Game::GetElapsed()
{
    return m_elapsed;
}

void Game::RestartClock()
{
    m_elapsed = m_clock.restart();
}

Window* Game::GetWindow()
{
    return &m_window;
}

void Game::Render()
{
    m_window.BeginDraw();
    m_stateManager.Draw();
    m_window.EndDraw();
}