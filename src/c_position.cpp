#include <SFML/System/Vector2.hpp>
#include <c_position.hpp>

C_Position::C_Position() : C_Base{Component::Position} {}

void C_Position::readInput(Utils::Tokens& l_tokens)
{
    std::tie(m_position.x, m_position.y) = *ConsumeTokens<float, float>(l_tokens);
}

const sf::Vector2f& C_Position::getPosition() const
{
    return m_position;
}

const sf::Vector2f& C_Position::getOldPosition() const
{
    return m_positionOld;
}

void C_Position::setPosition(const sf::Vector2f& l_pos)
{
    m_positionOld = m_position;
    m_position = l_pos;
}

void C_Position::moveBy(const sf::Vector2f& l_delta)
{
    m_positionOld = m_position;
    m_position += l_delta;
}