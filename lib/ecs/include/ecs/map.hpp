#ifndef ECS_MAP_HPP
#define ECS_MAP_HPP

#include <SFML/System/Vector2.hpp>
#include <core/graphics/tiles.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/map.fwd.hpp>
#include <ecs/shared_context.hpp>
#include <ecs/state/basestate.hpp>
#include <optional>
#include <unordered_map>

namespace ecs
{
using TileSheet = std::unordered_map<core::graphics::TileId, core::graphics::TileInfo>;
using TileMap   = std::unordered_map<core::graphics::TileId, core::graphics::Tile>;

class Map
{
  public:
    Map(SharedContext& l_context, state::BaseState& l_currentState);
    void loadMap(const std::string& l_path);
    void update(float l_dt);
    void draw();
    auto getPlayerId() -> ecs::EntityId;
    auto getTile(size_t iRow, size_t iCol) -> const core::graphics::Tile*;
    auto getTileSheetConfig() const -> const core::graphics::TileSheetConfig&;
    auto getMapSize() const -> const sf::Vector2u&;
    auto getGravity() const -> float;

  private:
    void loadTileset(const std::string& l_path);
    auto convertCoordinates(size_t iRow,
                            size_t iCol) const -> std::optional<core::graphics::TileId>;
    auto convertCoordinates(core::graphics::TileId l_id) -> sf::Vector2u;

    SharedContext&                  m_context;
    state::BaseState&               m_currentState;
    TileSheet                       m_tileSet;
    core::graphics::TileSheetConfig m_tileSheetConfig;
    TileMap                         m_tileMap;
    sf::Vector2u                    m_mapSize;
    sf::Sprite                      m_background;
    std::shared_ptr<sf::Texture>    m_backgroundTexture;
    std::optional<ecs::EntityId>    m_playerId;
    float                           m_gravity{512.};
};
} // namespace ecs

#endif