#ifndef ECS_ENTITY_C_POSITION_HPP
#define ECS_ENTITY_C_POSITION_HPP

#include <ecs/entity/c_base.hpp>
#include <SFML/System/Vector2.hpp>

namespace ecs::entity
{
class CPosition : public CBase
{
public:
    CPosition();

    void readInput(utils::Tokens& l_tokens) override;

    [[nodiscard]] auto getPosition() const -> const sf::Vector2f&;
    [[nodiscard]] auto getOldPosition() const -> const sf::Vector2f&;

    void setPosition(const sf::Vector2f& l_pos);

    void moveBy(const sf::Vector2f& l_delta);

private:
    sf::Vector2f m_position;
    sf::Vector2f m_positionOld;
};
} // namespace ecs::entity

#endif