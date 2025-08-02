#include <gamestate.hpp>

#include <eventmanager.hpp>
#include <statemanager.hpp>
#include <window.hpp>

GameState::GameState(StateManager& l_stateManager): BaseState(l_stateManager), m_world(std::get<0>(m_stateManager.GetContext()).GetWindowSize())
{
}

GameState::~GameState()
{
}

void GameState::Update(const sf::Time& l_elapsed)
{
    m_elapsed += l_elapsed;

    float timestep = 1.0f / 15.0f;

    if(m_elapsed.asSeconds() >= timestep)
    {
        m_world.Update();
        m_elapsed -= sf::seconds(timestep);
    }
}

void GameState::Draw()
{
    auto& [window, _] = m_stateManager.GetContext();

    m_world.Render(*window.GetRenderWindow());
}

void GameState::Activate()
{
}

void GameState::Deactivate()
{
}