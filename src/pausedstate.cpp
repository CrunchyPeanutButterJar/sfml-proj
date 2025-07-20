#include <pausedstate.hpp>

#include <eventmanager.hpp>
#include <statemanager.hpp>

PausedState::PausedState(StateManager& l_stateManager) : BaseState(l_stateManager)
{
    SetTransparent(true);

    auto& [_, eventManager] = m_stateManager.GetContext();
    eventManager.AddCallback(StateType::Paused, "Key_Escape", [&](const Events&){m_stateManager.SwitchTo(StateType::Game);});
}

PausedState::~PausedState()
{
    auto& [_, eventManager] = m_stateManager.GetContext();

    eventManager.RemoveCallback(StateType::Paused, "Key_Escape");
}

void PausedState::Draw()
{
    
}