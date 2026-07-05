#ifndef MAP_HPP
#define MAP_HPP

#include <ecs/ecs_types.hpp>
#include <core/graphics/tiles.hpp>
#include <unordered_map>
#include <basestate.hpp>
#include <sharedContext.hpp>
#include <SFML/System/Vector2.hpp>
#include <optional>

using TileSheet = std::unordered_map<core::graphics::TileId, core::graphics::TileInfo>;
using TileMap = std::unordered_map<core::graphics::TileId, core::graphics::Tile>;

class Map
{
public:
    Map(SharedContext& l_context, BaseState& l_currentState);
    void loadMap(const std::string& l_path);
    void update(float l_dt);
    void draw();
    ecs::EntityId getPlayerId();

private:
    void loadTileset(const std::string& l_path);
    core::graphics::TileId convertCoordinates(size_t iRow, size_t iCol);
    sf::Vector2u convertCoordinates(core::graphics::TileId l_id);


    SharedContext& m_context;
    BaseState& m_currentState;
    TileSheet m_tileSet;
    core::graphics::TileSheetConfig m_tileSheetConfig;
    TileMap m_tileMap;
    sf::Vector2u m_mapSize;
    sf::Sprite m_background;
    std::shared_ptr<sf::Texture> m_backgroundTexture;
    std::optional<ecs::EntityId> m_playerId;
};

#endif