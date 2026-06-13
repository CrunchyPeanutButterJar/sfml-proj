#include <pausedstate.hpp>

#include <eventmanager.hpp>
#include <statemanager.hpp>
#include <window.hpp>

PausedState::PausedState(StateManager& l_stateManager) : BaseState(l_stateManager)
{
    SetTransparent(true);

    auto& [window, eventManager] = m_stateManager.GetContext();
    
    eventManager.AddCallback(StateType::Paused, "Key_Escape", [&](const Events&){m_stateManager.SwitchTo(StateType::Game);});

    const auto windowSize = window.GetRenderWindow()->getSize();

    m_backdrop.setSize(sf::Vector2f(windowSize));
    m_backdrop.setPosition(0,0);
    m_backdrop.setFillColor(sf::Color(0,0,0,128));
}

PausedState::~PausedState()
{
    auto& [_, eventManager] = m_stateManager.GetContext();

    eventManager.RemoveCallback(StateType::Paused, "Key_Escape");
}

void PausedState::Draw()
{
    auto* window = m_stateManager.GetContext().m_window.GetRenderWindow();
    window->draw(m_backdrop);
}