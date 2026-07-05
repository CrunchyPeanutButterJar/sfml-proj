#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <core/directions.hpp>
#include <ecs/entity/c_movable.hpp>

static auto maxAbs(float l_x, float l_m /* > 0*/) -> float
{
    if (std::abs(l_x) > l_m)
    {
        return std::copysign(l_m, l_x);
    }

    return l_x;
}

static auto subtract(float l_op1, float l_op2) -> float
{
    float result = std::abs(l_op1) - std::abs(l_op2);

    if (std::copysign(1, result) != std::copysign(1, l_op2))
    {
        return 0;
    }

    return std::copysign(result, l_op1);
}

namespace ecs::entity
{
CMovable::CMovable() : CBase(Component::Movable) {}

void CMovable::readInput(utils::Tokens& l_tokens)
{
    unsigned int direction{0};
    std::tie(m_maxVelocity, m_speed.x, m_speed.y, direction) =
        *consumeTokens<float, float, float, unsigned int>(l_tokens);
    m_direction = static_cast<core::Direction>(direction);
}

void CMovable::addVelocity(const sf::Vector2f& l_vec)
{
    m_velocity += l_vec;
    m_velocity = {maxAbs(m_velocity.x, m_maxVelocity), maxAbs(m_velocity.y, m_maxVelocity)};
}

void CMovable::applyFriction(const sf::Vector2f& l_vec)
{
    auto [x, y] = m_velocity;
    m_velocity  = {subtract(x, l_vec.x), subtract(y, l_vec.y)};
}

void CMovable::accelerate(const sf::Vector2f& l_vec)
{
    m_acceleration += l_vec;
}

void CMovable::setAcceleration(const sf::Vector2f& l_vec)
{
    m_acceleration = l_vec;
}

void CMovable::setVelocity(const sf::Vector2f& l_vec)
{
    m_velocity = l_vec;
}

void CMovable::setDirection(core::Direction l_dir)
{
    m_direction = l_dir;
}

void CMovable::move(core::Direction l_dir)
{
    switch (l_dir)
    {
    case core::Direction::Left:
        m_acceleration.x -= m_speed.x;
        break;
    case core::Direction::Right:
        m_acceleration.x += m_speed.x;
        break;
    }
}

auto CMovable::getVelocity() const -> const sf::Vector2f&
{
    return m_velocity;
}

auto CMovable::getAcceleration() const -> const sf::Vector2f&
{
    return m_acceleration;
}

auto CMovable::getMaxVelocity() const -> float
{
    return m_maxVelocity;
}

} // namespace ecs::entity