#include <SFML/Graphics/Rect.hpp>
#include <core/window.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/map.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

using namespace core::graphics;
using namespace ecs;

Map::Map(SharedContext& l_context, state::BaseState& l_currentState)
    : m_context(l_context), m_currentState(l_currentState)
{
}

auto Map::convertCoordinates(TileId l_id) -> sf::Vector2u
{
    ASSERT(m_mapSize.x != 0 && m_mapSize.y != 0, "Map dimensions invalid");
    const auto [nRows, nCols] = m_mapSize;

    TileId i_row = l_id < nCols ? 0 : l_id % nCols;
    TileId i_col = l_id - i_row * nCols;

    return {i_row, i_col};
}

auto Map::convertCoordinates(size_t iRow, size_t iCol) -> TileId
{
    ASSERT(m_mapSize.x != 0 && m_mapSize.y != 0, "Map dimensions invalid");
    const auto [nRows, nCols] = m_mapSize;

    ASSERT(iRow < nRows && iCol < nCols, "Coordinates out of bounds! {} {}", iRow, iCol);
    return iRow * nCols + iCol;
}

void Map::loadMap(const std::string& l_path)
{
    auto map_file = utils::readFile(l_path);
    ASSERT(map_file.has_value(), "Error reading map file {}", l_path);
    utils::Tokens tokens{std::move(*map_file)};

    while (!tokens.empty())
    {
        auto key = *consumeToken<std::string>(tokens);
        if (key == "TilesSet")
        {
            loadTileset(utils::getConfigDirectory() + *consumeToken<std::string>(tokens));
        }
        else if (key == "Background")
        {
            std::string texture_alias = *consumeToken<std::string>(tokens);
            ASSERT_NON_FATAL(m_backgroundTexture == nullptr, "Overriding background texture");
            m_backgroundTexture = m_context.m_textureManager.acquire(texture_alias);
            ASSERT_NON_FATAL(m_backgroundTexture != nullptr,
                             "Could not load background texture with alias {}", texture_alias);
            if (m_backgroundTexture != nullptr)
            {
                m_background.setTexture(*m_backgroundTexture);
                const auto TextureSize = m_backgroundTexture->getSize();
                const auto ViewSize    = m_currentState.getView().getSize();
                m_background.setScale({ViewSize.x / TextureSize.x, ViewSize.y / TextureSize.y});
            }
        }
        else if (key == "Gravity")
        {
            *consumeToken<float>(tokens); // discard for now
        }
        else if (key == "Size")
        {
            std::tie(m_mapSize.x, m_mapSize.y) = *consumeTokens<size_t, size_t>(tokens);
        }
        else if (key == "TilesStart")
        {
            std::string temp;
            while (temp = *tokens.head<std::string>(), temp != "TilesEnd")
            {
                auto [tileId, iRow, iCol] = *consumeTokens<TileId, size_t, size_t>(tokens);
                auto it                   = m_tileSet.find(tileId);
                ASSERT_NON_FATAL(it != m_tileSet.end(), "Invalid tile id {}", tileId);
                if (it != m_tileSet.end())
                {
                    TileInfo& tile_info = it->second;
                    ASSERT_NON_FATAL(
                        m_tileMap.emplace(convertCoordinates(iRow, iCol), Tile{&tile_info}).second,
                        "Overriding existing tile at coordinates ({}, {})", iRow, iCol);
                }
            }
            tokens.advance();
        }
        else if (key == "EntitiesStart")
        {
            std::string temp;
            while (temp = *tokens.head<std::string>(), temp != "EntitiesEnd")
            {
                auto  name     = *consumeToken<std::string>(tokens);
                auto& entities = m_context.m_entityManager;
                int   entity   = m_context.m_entityManager.addEntity(name);
                if (entity < 0)
                {
                    FAILURE_NON_FATAL("Failed to load entity {}", name);
                    tokens.skipLine();
                    continue;
                }
                if (name == "player.entity")
                {
                    m_playerId = entity;
                }

                auto* position =
                    entities.getComponent<ecs::entity::CPosition>(entity, ecs::Component::Position);
                if (position != nullptr)
                {
                    position->readInput(tokens);
                }
            }
            tokens.advance();
        }
    }
}

void Map::loadTileset(const std::string& l_path)
{
    auto tile_set_file = utils::readFile(l_path);
    ASSERT(tile_set_file.has_value(), "Error reading tileset file {}", l_path);
    utils::Tokens tokens{std::move(*tile_set_file)};
    bool          filled_dimensions = false;
    bool          filled_tile_size  = false;
    bool          filled_texture    = false;

    while (!tokens.empty())
    {
        auto key = *consumeToken<std::string>(tokens);
        if (key == "Dimensions")
        {
            ASSERT_NON_FATAL(!filled_dimensions, "Overriding tilesheet dimensions in {}", l_path);
            std::tie(m_tileSheetConfig.m_width, m_tileSheetConfig.m_height) =
                *consumeTokens<size_t, size_t>(tokens);
            filled_dimensions = true;
        }
        else if (key == "TileSize")
        {
            ASSERT_NON_FATAL(!filled_tile_size, "Overriding tilesheet tilesize in {}", l_path);
            std::tie(m_tileSheetConfig.m_tileSize) = *consumeTokens<size_t>(tokens);
            filled_tile_size                       = true;
        }
        else if (key == "Texture")
        {
            ASSERT_NON_FATAL(!filled_texture, "Overriding tilesheet texture in {}", l_path);
            std::string texture_alias   = *consumeToken<std::string>(tokens);
            m_tileSheetConfig.m_texture = m_context.m_textureManager.acquire(texture_alias);
            ASSERT_NON_FATAL(m_tileSheetConfig.m_texture != nullptr,
                             "Error loading texture alias {}", texture_alias);
            filled_texture = true;
        }
        else if (key == "TilesStart")
        {
            ASSERT(filled_dimensions && filled_tile_size && filled_texture, "Invalid config for {}",
                   l_path);
            std::string tile_type;
            while (std::tie(tile_type) = *consumeTokens<std::string>(tokens),
                   tile_type != "TilesEnd")
            {
                TileInfo new_tile{m_tileSheetConfig, tokens};
                auto     tile_id = new_tile.m_id;
                ASSERT_NON_FATAL(m_tileSet.insert({tile_id, std::move(new_tile)}).second,
                                 "Overriden Tile id {} from config file {}", tile_id, l_path);
            }
        }
    }

    ASSERT(filled_dimensions && filled_tile_size && filled_texture,
           "Missing data in tilesheet config file {}", l_path);
}

void Map::update(float /*unused*/)
{
    sf::FloatRect view_space = m_context.m_window.getViewSpace();
    m_background.setPosition(view_space.left, view_space.top);
}

void Map::draw()
{
    auto* window = m_context.m_window.getRenderWindow();
    window->draw(m_background);
    const sf::FloatRect ViewSpace = m_context.m_window.getViewSpace();

    float x = ViewSpace.left;
    float y = ViewSpace.top;

    const auto [nRow, nCols] = m_mapSize;
    const size_t TileSize    = m_tileSheetConfig.m_tileSize;

    TileId i_row_begin = std::floor(y / TileSize);
    TileId i_col_begin = std::ceil(x / TileSize);

    for (TileId i_row = i_row_begin; i_row < nRow; i_row++)
    {
        for (TileId i_col = i_col_begin; i_col < nCols; i_col++)
        {
            if (auto it = m_tileMap.find(convertCoordinates(i_row, i_col)); it != m_tileMap.end())
            {
                auto& tile      = it->second;
                auto& tile_info = *tile.m_tileInfo;
                tile_info.m_sprite.setPosition({(float)i_col * TileSize, (float)i_row * TileSize});
                window->draw(tile_info.m_sprite);
            }
        }
    }
}

auto Map::getPlayerId() -> ecs::EntityId
{
    ASSERT(m_playerId.has_value(), "did not load player id");
    return *m_playerId;
}