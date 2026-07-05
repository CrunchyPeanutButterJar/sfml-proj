#include <basestate.hpp>
#include <statemanager.hpp>
#include <core/window.hpp>

BaseState::BaseState(StateManager& l_stateManager) : m_stateManager(l_stateManager), m_view(m_stateManager.getContext().m_window.getRenderWindow()->getDefaultView())
{
    
}