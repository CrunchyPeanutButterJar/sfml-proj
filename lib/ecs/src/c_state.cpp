#include "utils/utilities.hpp"
#include <ecs/entity/c_state.hpp>

namespace ecs::entity
{

CState::CState() : CBase{Component::State} {}

void CState::readInput(utils::Tokens& l_tokens)
{
    setState((EntityState)*consumeToken<unsigned int>(l_tokens));
}

auto CState::getState() const -> EntityState
{
    return m_state;
}

void CState::setState(EntityState l_state)
{
    m_state = l_state;
}

} // namespace ecs::entity