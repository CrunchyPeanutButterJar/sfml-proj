#include <core/state/basestate.hpp>
#include <core/state/statemanager.hpp>
#include <core/window.hpp>

using namespace core::state;

BaseState::BaseState(StateManager& l_stateManager)
    : m_stateManager(l_stateManager),
      m_view(m_stateManager.getContext()->m_window.getRenderWindow()->getDefaultView())
{
}