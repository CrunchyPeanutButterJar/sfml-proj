#include "core/graphics/tiles.hpp"
#include "ecs/ecs_types.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/System/Vector2.hpp>
#include <ecs/entity/c_collidable.hpp>

namespace ecs::entity
{
CCollidable::CCollidable() : CDrawable{Component::Collidable} {}

void CCollidable::readInput(utils::Tokens& l_tokens)
{
    unsigned int origin = 0;
    sf::Vector2f size;

    std::tie(size.x, size.y, m_offset.x, m_offset.y, origin) =
        *consumeTokens<float, float, float, float, unsigned int>(l_tokens);

    setSize(size);
    m_origin = (Origin)origin;
}

void CCollidable::collideOnX()
{
    m_collidingOnX = true;
}

void CCollidable::collideOnY(const core::graphics::Tile* l_tile)
{
    m_collidingOnY = true;
    m_groundTile   = l_tile;
}

auto CCollidable::getGroundTile() const -> const core::graphics::Tile*
{
    return m_groundTile;
}

void CCollidable::resetCollisionFlags()
{
    m_groundTile   = nullptr;
    m_collidingOnX = false;
    m_collidingOnY = false;
}

void CCollidable::setSize(const sf::Vector2f& l_vec)
{
    m_aabb.width  = l_vec.x;
    m_aabb.height = l_vec.y;

    m_size.x = (unsigned int)l_vec.x;
    m_size.y = (unsigned int)l_vec.y;
}

void CCollidable::setPosition(const sf::Vector2f& l_vec)
{
    switch (m_origin)
    {
    case Origin::Top_Left:
    {
        m_aabb.left = l_vec.x + m_offset.x;
        m_aabb.top  = l_vec.y + m_offset.y;
        break;
    }
    case Origin::Abs_Centre:
    {
        m_aabb.left = l_vec.x - (m_aabb.width / 2) + m_offset.x;
        m_aabb.top  = l_vec.y - (m_aabb.height / 2) + m_offset.y;
        break;
    }
    case Origin::Mid_Bottom:
    {
        m_aabb.left = l_vec.x - (m_aabb.width / 2) + m_offset.x;
        m_aabb.top  = l_vec.y - m_aabb.height + m_offset.y;
        break;
    }
    }
}

auto CCollidable::getCollidable() const -> const sf::FloatRect&
{
    return m_aabb;
}

void CCollidable::updatePosition(const sf::Vector2f& l_vec)
{
    setPosition(l_vec);
}

auto CCollidable::getSize() -> const sf::Vector2u&
{
    return m_size;
}

bool CCollidable::debug_overlay{false};

void CCollidable::draw(sf::RenderWindow* l_window)
{
    if (!debug_overlay)
    {
        return;
    }

    sf::RectangleShape outline{m_aabb.getSize()};
    outline.setOutlineColor(sf::Color::Blue);
    outline.setOutlineThickness(3.);
    outline.setFillColor(sf::Color::Transparent);
    outline.setPosition(m_aabb.getPosition());

    l_window->draw(outline);
}

} // namespace ecs::entity