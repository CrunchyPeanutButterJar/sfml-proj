#include <core/window.hpp>
#include <ecs/state/basestate.hpp>
#include <ecs/state/statemanager.hpp>

using namespace ecs::state;

BaseState::BaseState(StateManager& l_stateManager)
    : m_stateManager(l_stateManager),
      m_view(m_stateManager.getContext().m_window.getRenderWindow()->getDefaultView())
{
}