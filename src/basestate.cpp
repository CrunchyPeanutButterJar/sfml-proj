#include <basestate.hpp>
#include <statemanager.hpp>
#include <window.hpp>

BaseState::BaseState(StateManager& l_stateManager) : m_stateManager(l_stateManager)
{
    m_view = m_stateManager.GetContext().m_window.GetRenderWindow()->getDefaultView();
}