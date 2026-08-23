#ifndef ECS_ENTITY_C_AICONTROLLER_HPP
#define ECS_ENTITY_C_AICONTROLLER_HPP

#include <SFML/System/Vector2.hpp>
#include <array>
#include <ecs/entity/c_base.hpp>

namespace ecs::entity
{
class CAIController : public CBase
{
  public:
    static constexpr size_t GOAL_COUNT = 2;

    using Goals = std::array<sf::Vector2f, GOAL_COUNT>;

    CAIController();

    void readInput(utils::Tokens& /*unused*/) override;

    [[nodiscard]] auto getActiveGoal() -> const sf::Vector2f&;
    [[nodiscard]] auto getGoals() -> const Goals&;

    void switchToNextGoal();

    void setGoals(const Goals& l_goals);

  private:
    Goals  m_goTo;
    size_t m_activeGoalIndex{0};
};
} // namespace ecs::entity

#endif