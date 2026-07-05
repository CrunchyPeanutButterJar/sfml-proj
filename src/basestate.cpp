#include <basestate.hpp>
#include <core/window.hpp>
#include <statemanager.hpp>

BaseState::BaseState(StateManager& l_stateManager)
    : m_stateManager(l_stateManager),
      m_view(m_stateManager.getContext().m_window.getRenderWindow()->getDefaultView())
{
}