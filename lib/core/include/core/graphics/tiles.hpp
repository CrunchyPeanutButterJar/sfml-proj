#ifndef CORE_GRAPHICS_TILES_HPP
#define CORE_GRAPHICS_TILES_HPP

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/graphics/texture_manager.hpp>
#include <core/graphics/tiles.fwd.hpp>
#include <utils/utilities.hpp>

#include <string>

namespace core::graphics
{

struct TileSheetConfig
{
    size_t                       m_tileSize{};
    size_t                       m_width{};
    size_t                       m_height{};
    std::shared_ptr<sf::Texture> m_texture;
};

struct TileInfo
{
    TileInfo(const TileSheetConfig& l_config, utils::Tokens& l_tokens);

    const TileSheetConfig& m_config;
    const TileId           m_id;
    sf::Sprite             m_sprite;
    const std::string      m_name;
    const sf::Vector2f     m_friction;
};

struct Tile
{
    TileInfo* m_tileInfo;
};
} // namespace core::graphics

#endif