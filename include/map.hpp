#ifndef MAP_HPP
#define MAP_HPP

#include <tiles.hpp>
#include <unordered_map>
#include <basestate.hpp>
#include <sharedContext.hpp>
#include <SFML/System/Vector2.hpp>

using TileSheet = std::unordered_map<TileId, TileInfo>;
using TileMap = std::unordered_map<TileId, Tile>;

class Map
{
public:
    Map(SharedContext& l_context, BaseState& l_currentState);
    void loadMap(const std::string& l_path);
    void update(float l_dt);
    void draw();

private:
    void loadTileset(const std::string& l_path);
    TileId convertCoordinates(size_t iRow, size_t iCol);
    sf::Vector2u convertCoordinates(TileId l_id);

private:
    SharedContext& m_context;
    BaseState& m_currentState;
    TileSheet m_tileSet;
    TileSheetConfig m_tileSheetConfig;
    TileMap m_tileMap;
    sf::Vector2u m_mapSize;
    sf::Sprite m_background;
    std::shared_ptr<sf::Texture> m_backgroundTexture;
};

#endif