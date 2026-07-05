#include <SFML/System/Vector2.hpp>
#include <ecs/entity/c_position.hpp>

CPosition::CPosition() : CBase{Component::Position} {}

void CPosition::readInput(Utils::Tokens& l_tokens)
{
    std::tie(m_position.x, m_position.y) = *consumeTokens<float, float>(l_tokens);
}

const sf::Vector2f& CPosition::getPosition() const
{
    return m_position;
}

const sf::Vector2f& CPosition::getOldPosition() const
{
    return m_positionOld;
}

void CPosition::setPosition(const sf::Vector2f& l_pos)
{
    m_positionOld = m_position;
    m_position = l_pos;
}

void CPosition::moveBy(const sf::Vector2f& l_delta)
{
    m_positionOld = m_position;
    m_position += l_delta;
}