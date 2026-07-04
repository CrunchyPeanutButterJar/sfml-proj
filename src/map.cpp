#include <SFML/Graphics/Rect.hpp>
#include <map.hpp>
#include <window.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>
#include <entitymanager.hpp>
#include <c_position.hpp>

Map::Map(SharedContext& l_context, BaseState& l_currentState):
m_context(l_context),
m_currentState(l_currentState)
{    
}

sf::Vector2u Map::convertCoordinates(TileId l_id)
{
    ASSERT(m_mapSize.x != 0 && m_mapSize.y != 0, "Map dimensions invalid");
    const auto [nRows, nCols] = m_mapSize;

    TileId iRow = l_id < nCols? 0 : l_id % nCols;
    TileId iCol = l_id - iRow * nCols;
    
    return {iRow, iCol};
}

TileId Map::convertCoordinates(size_t iRow, size_t iCol)
{
    ASSERT(m_mapSize.x != 0 && m_mapSize.y != 0, "Map dimensions invalid");
    const auto [nRows, nCols] = m_mapSize;
    
    ASSERT(iRow < nRows && iCol < nCols, "Coordinates out of bounds! {} {}", iRow, iCol);
    return iRow * nCols + iCol;
}

void Map::loadMap(const std::string& l_path)
{
    auto mapFile = Utils::ReadFile(l_path);
    ASSERT(mapFile.has_value(), "Error reading map file {}", l_path);
    Utils::Tokens tokens{std::move(*mapFile)};
    
    while(!tokens.empty())
    {
        auto key = *ConsumeToken<std::string>(tokens);
        if(key == "TilesSet")
        {
            loadTileset(Utils::GetConfigDirectory() + *ConsumeToken<std::string>(tokens));
        }
        else if(key == "Background")
        {
            std::string textureAlias = *ConsumeToken<std::string>(tokens);
            ASSERT_NON_FATAL(m_backgroundTexture == nullptr, "Overriding background texture");
            m_backgroundTexture = m_context.m_textureManager.acquire(textureAlias);
            ASSERT_NON_FATAL(m_backgroundTexture != nullptr, "Could not load background texture with alias {}", textureAlias);
            if(m_backgroundTexture != nullptr)
            {
                m_background.setTexture(*m_backgroundTexture);
                const auto textureSize = m_backgroundTexture->getSize();
                const auto viewSize = m_currentState.GetView().getSize();
                m_background.setScale( {viewSize.x / textureSize.x, viewSize.y / textureSize.y} );
            }
        }
        else if(key == "Gravity")
        {
            *ConsumeToken<float>(tokens);//discard for now
        }
        else if (key == "Size")
        {
            std::tie(m_mapSize.x, m_mapSize.y) = *ConsumeTokens<size_t, size_t>(tokens);
        }
        else if(key == "TilesStart")
        {
            std::string temp;
            while(temp = *tokens.head<std::string>(), temp != "TilesEnd")
            {
                auto [tileId, iRow, iCol] = *ConsumeTokens<TileId, size_t, size_t>(tokens);
                auto it = m_tileSet.find(tileId);
                ASSERT_NON_FATAL(it != m_tileSet.end(), "Invalid tile id {}", tileId);
                if(it != m_tileSet.end())
                {
                    TileInfo& tileInfo = it->second;
                    ASSERT_NON_FATAL(m_tileMap.emplace(convertCoordinates(iRow, iCol), Tile{&tileInfo}).second,
                "Overriding existing tile at coordinates ({}, {})", iRow, iCol);
                }
            }
            tokens.advance();
        }
        else if(key == "EntitiesStart")
        {
            std::string temp;
            while(temp = *tokens.head<std::string>(), temp != "EntitiesEnd")
            {
                auto name = *ConsumeToken<std::string>(tokens);
                auto& entities = m_context.m_entityManager;
                int entity = m_context.m_entityManager.addEntity(name);
                if(entity < 0 )
                {
                    FAILURE_NON_FATAL("Failed to load entity {}", name);
                    tokens.skipLine();
                    continue;
                }
                if(name == "player.entity")
                {
                    m_playerId = entity;
                }

                auto* position = entities.getComponent<C_Position>(entity, Component::Position);
                if(position)
                {
                    position->readInput(tokens);
                }
            }
            tokens.advance();
        }
    }
}

void Map::loadTileset(const std::string &l_path)
{
    auto tileSetFile = Utils::ReadFile(l_path);
    ASSERT(tileSetFile.has_value(), "Error reading tileset file {}", l_path);
    Utils::Tokens tokens{std::move(*tileSetFile)};
    bool filledDimensions = false, filledTileSize = false, filledTexture = false; 

    while(!tokens.empty())
    {
        auto key = *ConsumeToken<std::string>(tokens);
        if(key == "Dimensions")
        {
            ASSERT_NON_FATAL(!filledDimensions, "Overriding tilesheet dimensions in {}", l_path);
            std::tie(m_tileSheetConfig.m_width, m_tileSheetConfig.m_height) = *ConsumeTokens<size_t, size_t>(tokens);
            filledDimensions = true;
        }
        else if(key == "TileSize")
        {
            ASSERT_NON_FATAL(!filledTileSize, "Overriding tilesheet tilesize in {}", l_path);
            std::tie(m_tileSheetConfig.m_tileSize) = *ConsumeTokens<size_t>(tokens);
            filledTileSize = true;
        }
        else if(key == "Texture")
        {
            ASSERT_NON_FATAL(!filledTexture, "Overriding tilesheet texture in {}", l_path);
            std::string textureAlias = *ConsumeToken<std::string>(tokens);
            m_tileSheetConfig.m_texture = m_context.m_textureManager.acquire(textureAlias);
            ASSERT_NON_FATAL(m_tileSheetConfig.m_texture != nullptr, "Error loading texture alias {}", textureAlias);
            filledTexture = true;
        }
        else if(key == "TilesStart")
        {
            ASSERT(filledDimensions && filledTileSize && filledTexture, "Invalid config for {}", l_path);
            std::string tileType;
            while(std::tie(tileType) = *ConsumeTokens<std::string>(tokens), tileType != "TilesEnd")
            {
                TileInfo newTile{m_tileSheetConfig, tokens};
                auto tileId = newTile.m_id;
                ASSERT_NON_FATAL(m_tileSet.insert({tileId, std::move(newTile)}).second, "Overriden Tile id {} from config file {}", tileId, l_path);
            }
        }
    }

    ASSERT(filledDimensions && filledTileSize && filledTexture, "Missing data in tilesheet config file {}", l_path);
}

void Map::update(float)
{
    sf::FloatRect viewSpace = m_context.m_window.GetViewSpace();
    m_background.setPosition(viewSpace.left, viewSpace.top);
}

void Map::draw()
{
    auto* window = m_context.m_window.GetRenderWindow();
    window->draw(m_background);
    const sf::FloatRect viewSpace = m_context.m_window.GetViewSpace();
    
    float x = viewSpace.left;
    float y = viewSpace.top;

    const auto [nRow, nCols] = m_mapSize;
    const size_t tileSize = m_tileSheetConfig.m_tileSize;

    TileId iRowBegin = std::floor(y / tileSize);
    TileId iColBegin = std::ceil(x / tileSize);

    for(TileId iRow = iRowBegin; iRow < nRow; iRow++)
    {
        for(TileId iCol = iColBegin; iCol < nCols ; iCol++)
        {
            if(auto it = m_tileMap.find(convertCoordinates(iRow, iCol)); it != m_tileMap.end())
            {
                auto& tile = it->second;
                auto& tileInfo = *tile.m_tileInfo;
                tileInfo.m_sprite.setPosition
                ({
                    (float)iCol*tileSize,
                    (float)iRow*tileSize
                });
                window->draw(tileInfo.m_sprite);
            }
        }
    }

}

EntityId Map::getPlayerId()
{
    ASSERT(m_playerId.has_value(), "did not load player id");
    return *m_playerId;
}