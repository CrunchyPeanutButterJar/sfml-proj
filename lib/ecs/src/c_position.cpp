#include <SFML/System/Vector2.hpp>
#include <ecs/entity/c_position.hpp>
#include <utils/utilities.hpp>

using namespace ecs::entity;

CPosition::CPosition() : CBase{Component::Position} {}

void CPosition::readInput(utils::Tokens& l_tokens)
{
    std::tie(m_position.x, m_position.y) = *consumeTokens<float, float>(l_tokens);
}

auto CPosition::getPosition() const -> const sf::Vector2f&
{
    return m_position;
}

auto CPosition::getOldPosition() const -> const sf::Vector2f&
{
    return m_positionOld;
}

void CPosition::setPosition(const sf::Vector2f& l_pos)
{
    m_positionOld = m_position;
    m_position    = l_pos;
}

void CPosition::moveBy(const sf::Vector2f& l_delta)
{
    m_positionOld = m_position;
    m_position += l_delta;
}