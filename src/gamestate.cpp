#include <gamestate.hpp>

#include <eventmanager.hpp>
#include <statemanager.hpp>
#include <window.hpp>

#include <SFML/Window/Keyboard.hpp>

GameState::GameState(StateManager& l_stateManager): BaseState(l_stateManager)
{
}

GameState::~GameState()
{
}

void GameState::Update(const sf::Time& l_elapsed)
{
    m_elapsed += l_elapsed;

    float timestep = 1.0f / 60;

    if(m_elapsed.asSeconds() >= timestep)
    {
    }
}

void GameState::Draw()
{
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