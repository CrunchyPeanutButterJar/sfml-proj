#ifndef ECS_MAP_HPP
#define ECS_MAP_HPP

#include <SFML/System/Vector2.hpp>
#include <core/graphics/gif.hpp>
#include <core/graphics/tiles.hpp>
#include <core/state/basestate.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/map.fwd.hpp>
#include <ecs/shared_context.hpp>
#include <optional>
#include <unordered_map>

namespace ecs
{
using TileSheet = std::unordered_map<core::graphics::TileId, core::graphics::TileInfo>;
using TileMap   = std::unordered_map<core::graphics::TileId, core::graphics::Tile>;

class Map
{
  public:
    Map(SharedContext* l_context, core::state::BaseState& l_currentState);
    void loadMap(const std::string& l_path);
    auto requiresTilesMapRegeneration() -> float;
    void update(float l_dt);
    void draw();
    auto getPlayerId() -> ecs::EntityId;
    auto getTile(int iRow, int iCol) -> const core::graphics::Tile*;
    auto getTileSheetConfig() const -> const core::graphics::TileSheetConfig&;
    auto getMapSize() const -> const sf::Vector2u&;
    auto getGravity() const -> float;
    void transitionToNextGif();

  private:
    auto getTile(size_t iRow, size_t iCol) -> const core::graphics::Tile*;
    void loadTileset(const std::string& l_path);
    auto convertCoordinates(size_t iRow,
                            size_t iCol) const -> std::optional<core::graphics::TileId>;
    auto convertCoordinates(core::graphics::TileId l_id) -> sf::Vector2u;
    auto convertCoordinates(int iRow, int iCol) const -> std::optional<sf::Vector2<size_t>>;

    void proceeduralTilesGeneration(size_t l_startRow, size_t l_endRow);
    auto generateTiles(size_t l_row, size_t l_col, size_t l_length) -> size_t;

    auto requiresProceeduralTilesReGeneration() -> bool;
    auto proceeduralTilesReGeneration() -> float;

    SharedContext*                   m_context;
    core::state::BaseState&          m_currentState;
    TileSheet                        m_tileSet;
    core::graphics::TileSheetConfig  m_tileSheetConfig;
    TileMap                          m_tileMap;
    sf::Vector2u                     m_mapSize;
    sf::Sprite                       m_background;
    std::shared_ptr<sf::Texture>     m_backgroundTexture;
    std::optional<ecs::EntityId>     m_playerId;
    float                            m_gravity{512.};
    std::vector<core::graphics::Gif> m_gifs;
    core::graphics::Gif*             m_currentGif{};
    core::graphics::Gif*             m_transitionGif{};
    core::graphics::Gif*             m_activeGif{};
    // Proceedural generation info
    size_t m_maxTilesLength{};
    size_t m_minTilesLength{};
    float  m_minTilesPercentageOnScreen{};
    size_t m_seed{};
    bool   m_seeded{false};
};
} // namespace ecs

#endif