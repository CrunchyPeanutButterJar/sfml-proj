#include <ecs/entity/c_aicontroller.hpp>

namespace ecs::entity
{
CAIController::CAIController() : CBase{ecs::Component::AIController} {}

void CAIController::readInput(utils::Tokens& /*unused*/) {}

auto CAIController::getActiveGoal() -> const sf::Vector2f&
{
    return m_goTo.at(m_activeGoalIndex);
}

void CAIController::switchToNextGoal()
{
    m_activeGoalIndex = (m_activeGoalIndex + 1) % GOAL_COUNT;
}

void CAIController::setGoals(const Goals& l_goals)
{
    m_goTo = l_goals;
}

} // namespace ecs::entity