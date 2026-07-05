#include <SFML/Graphics/Rect.hpp>
#include <core/graphics/tiles.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

TileInfo::TileInfo(const TileSheetConfig& l_config, Utils::Tokens& l_tokens)
:
m_config{l_config},
m_id{*ConsumeToken<TileId>(l_tokens)},
m_name{*ConsumeToken<std::string>(l_tokens)},
m_friction{*ConsumeToken<float>(l_tokens), *ConsumeToken<float>(l_tokens)}
{
    const size_t nRows = m_config.m_height / m_config.m_tileSize;
    const size_t nCols = m_config.m_width / m_config.m_tileSize;
    const size_t iRow = m_id < nCols ? 0 : m_id % nCols;
    const size_t iCol = m_id - iRow * nCols;

    const int x = iCol * m_config.m_tileSize;
    const int y = iRow * m_config.m_tileSize;
    ASSERT_NON_FATAL(iRow < nRows, "invalid Tile config for {}", m_name);

    sf::IntRect tileBounds{x, y, (int)m_config.m_tileSize, (int)m_config.m_tileSize};
    m_sprite.setTexture(*m_config.m_texture);
    m_sprite.setTextureRect(tileBounds);
}