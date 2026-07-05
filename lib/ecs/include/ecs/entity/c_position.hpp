#ifndef C_POSITION_HPP
#define C_POSITION_HPP

#include <ecs/entity/c_base.hpp>
#include <SFML/System/Vector2.hpp>

class C_Position : public C_Base
{
public:
    C_Position();

    void readInput(Utils::Tokens& l_tokens) override;

    const sf::Vector2f& getPosition() const;
    const sf::Vector2f& getOldPosition() const;

    void setPosition(const sf::Vector2f& l_pos);

    void moveBy(const sf::Vector2f& l_delta);

private:
    sf::Vector2f m_position;
    sf::Vector2f m_positionOld;
};

#endif