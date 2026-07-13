#ifndef ECS_ENTITY_C_MOVABLE_HPP
#define ECS_ENTITY_C_MOVABLE_HPP

#include <SFML/System/Vector2.hpp>
#include <core/directions.hpp>
#include <ecs/entity/c_base.hpp>
#include <ecs/entity/c_movable.fwd.hpp>

namespace ecs::entity
{
class CMovable : public CBase
{
  public:
    CMovable();

    void readInput(utils::Tokens& l_tokens) override;

    void addVelocity(const sf::Vector2f& l_vec);
    void applyFriction(const sf::Vector2f& l_vec);
    void accelerate(const sf::Vector2f& l_vec);

    void move(core::Direction l_dir);
    void jump();

    void setAcceleration(const sf::Vector2f& l_vec);
    void setVelocity(const sf::Vector2f& l_vec);
    void setDirection(core::Direction l_dir);

    [[nodiscard]] auto getVelocity() const -> const sf::Vector2f&;
    [[nodiscard]] auto getAcceleration() const -> const sf::Vector2f&;
    [[nodiscard]] auto getMaxVelocity() const -> float;

  private:
    sf::Vector2f    m_velocity;
    float           m_maxVelocity{};
    sf::Vector2f    m_speed;
    sf::Vector2f    m_acceleration;
    core::Direction m_direction{};
};
} // namespace ecs::entity

#endif