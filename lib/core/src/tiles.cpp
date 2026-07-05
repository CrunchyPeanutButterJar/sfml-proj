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
    const size_t n_rows = m_config.m_height / m_config.m_tileSize;
    const size_t n_cols = m_config.m_width / m_config.m_tileSize;
    const size_t i_row = m_id < n_cols ? 0 : m_id % n_cols;
    const size_t i_col = m_id - i_row * n_cols;

    const int x = i_col * m_config.m_tileSize;
    const int y = i_row * m_config.m_tileSize;
    ASSERT_NON_FATAL(i_row < n_rows, "invalid Tile config for {}", m_name);

    sf::IntRect tile_bounds{x, y, (int)m_config.m_tileSize, (int)m_config.m_tileSize};
    m_sprite.setTexture(*m_config.m_texture);
    m_sprite.setTextureRect(tile_bounds);
}