#include <SFML/Graphics/Rect.hpp>
#include <core/graphics/tiles.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

TileInfo::TileInfo(const TileSheetConfig& l_config, Utils::Tokens& l_tokens)
:
m_config{l_config},
m_id{*consumeToken<TileId>(l_tokens)},
m_name{*consumeToken<std::string>(l_tokens)},
m_friction{*consumeToken<float>(l_tokens), *consumeToken<float>(l_tokens)}
{
    const size_t NRows = m_config.m_height / m_config.m_tileSize;
    const size_t NCols = m_config.m_width / m_config.m_tileSize;
    const size_t IRow = m_id < NCols ? 0 : m_id % NCols;
    const size_t ICol = m_id - IRow * NCols;

    const int X = ICol * m_config.m_tileSize;
    const int Y = IRow * m_config.m_tileSize;
    ASSERT_NON_FATAL(IRow < NRows, "invalid Tile config for {}", m_name);

    sf::IntRect tile_bounds{X, Y, (int)m_config.m_tileSize, (int)m_config.m_tileSize};
    m_sprite.setTexture(*m_config.m_texture);
    m_sprite.setTextureRect(tile_bounds);
}