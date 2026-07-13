#ifndef ECS_ENTITY_C_COLLIDABLE_HPP
#define ECS_ENTITY_C_COLLIDABLE_HPP

#include <ecs/entity/c_collidable.fwd.hpp>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/graphics/tiles.fwd.hpp>
#include <cstdint>
#include <ecs/entity/c_drawable.hpp>

namespace ecs::entity
{

enum class Origin : std::uint8_t
{
    Top_Left,
    Abs_Centre,
    Mid_Bottom
};

class CCollidable : public CDrawable
{
  public:
    CCollidable();

    void readInput(utils::Tokens& l_tokens) override;

    void collideOnX();
    void collideOnY(const core::graphics::Tile* l_tile);
    void resetCollisionFlags();

    void setSize(const sf::Vector2f& l_vec);
    void setPosition(const sf::Vector2f& l_vec);

    [[nodiscard]] auto getCollidable() const -> const sf::FloatRect&;
    [[nodiscard]] auto getGroundTile() const -> const core::graphics::Tile*;

    void updatePosition(const sf::Vector2f& l_vec) override;
    auto getSize() -> const sf::Vector2u& override;
    void draw(sf::RenderWindow* l_window) override;

    static bool debug_overlay;

  private:
    sf::FloatRect m_aabb;
    sf::Vector2u  m_size;
    sf::Vector2f  m_offset;
    Origin        m_origin{Origin::Mid_Bottom};

    const core::graphics::Tile* m_groundTile{nullptr};

    bool m_collidingOnX{false};
    bool m_collidingOnY{false};
};
} // namespace ecs::entity

#endif