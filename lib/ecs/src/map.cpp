#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/graphics/gif.hpp>
#include <core/window.hpp>
#include <cstdlib>
#include <ctime>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/map.hpp>
#include <ecs/messaging/entity_message.hpp>
#include <ecs/messaging/message.hpp>
#include <ecs/messaging/message_handler.hpp>
#include <ecs/system/system_manager.hpp>
#include <optional>
#include <sstream>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

using namespace core::graphics;
using namespace ecs;

static constexpr TileId LEFT   = 12;
static constexpr TileId MIDDLE = 13;
static constexpr TileId RIGHT  = 14;
static constexpr TileId ALONE  = 15;

Map::Map(SharedContext* l_context, core::state::BaseState& l_currentState)
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

auto Map::convertCoordinates(size_t iRow, size_t iCol) const -> std::optional<TileId>
{
    ASSERT_DEBUG_BUILD(m_mapSize.x != 0 && m_mapSize.y != 0, "Map dimensions invalid");
    const auto [nRows, nCols] = m_mapSize;
    if (iRow >= nRows || iCol >= nCols)
    {
        return std::nullopt;
    }

    return (iRow * nCols) + iCol;
}

static auto getUpscaleFactors(const sf::Vector2f& l_originalSize,
                              const sf::Vector2f& l_targetSize) -> sf::Vector2f
{
    return {l_targetSize.x / l_originalSize.x, l_targetSize.y / l_originalSize.y};
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
            m_backgroundTexture = m_context->m_textureManager.acquire(texture_alias);
            ASSERT_NON_FATAL(m_backgroundTexture != nullptr,
                             "Could not load background texture with alias {}", texture_alias);
            if (m_backgroundTexture != nullptr)
            {
                m_background.setTexture(*m_backgroundTexture);
                m_background.setScale(
                    getUpscaleFactors(sf::Vector2f(m_backgroundTexture->getSize()),
                                      m_currentState.getView().getSize()));
            }
        }
        else if (key == "Gravity")
        {
            m_gravity = *consumeToken<float>(tokens);
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
                    TileInfo& tile_info  = it->second;
                    auto      tile_index = convertCoordinates(iRow, iCol);
                    ASSERT(tile_index.has_value(), "");
                    ASSERT_NON_FATAL(m_tileMap.emplace(tile_index.value(), Tile{&tile_info}).second,
                                     "Overriding existing tile at coordinates ({}, {})", iRow,
                                     iCol);
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
                auto& entities = m_context->m_entityManager;
                int   entity   = m_context->m_entityManager.addEntity(name);
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
        else if (key == "ProceeduralGenerationConfigStart")
        {
            while (tokens.head<std::string>().value() != "ProceeduralGenerationConfigEnd")
            {
                key = *consumeToken<std::string>(tokens);
                if (key == "Gifs")
                {
                    tokens.captureQuotedStrings('"');
                    auto str = *consumeToken<std::string>(tokens);
                    tokens.captureQuotedStrings({});
                    utils::Tokens gifs{std::istringstream{std::move(str)}};
                    while (!gifs.empty())
                    {
                        const auto          GifConfigFileName = *consumeToken<std::string>(gifs);
                        core::graphics::Gif temp{m_context->m_textureManager};
                        const std::string   Path = "media/gifs/" + GifConfigFileName + ".gifconf";
                        ASSERT(temp.loadGif(Path), "Failure loading gif file");
                        auto [width_scalar, height_scalar] = getUpscaleFactors(
                            sf::Vector2f(temp.getImageSize()), m_currentState.getView().getSize());
                        temp.scale(width_scalar, height_scalar);
                        m_gifs.emplace_back(std::move(temp));
                    }

                    m_currentGif    = &m_gifs.front();
                    m_transitionGif = &m_gifs.back();
                    m_activeGif     = m_currentGif;
                }
                else if (key == "MinTilesLength")
                {
                    m_minTilesLength = *consumeToken<size_t>(tokens);
                }
                else if (key == "MaxTilesLength")
                {
                    m_maxTilesLength = *consumeToken<size_t>(tokens);
                }
                else if (key == "MinTilesPercentageOnScreen")
                {
                    m_minTilesPercentageOnScreen = *consumeToken<float>(tokens);
                }
                else if (key == "Seed")
                {
                    m_seed = *consumeToken<size_t>(tokens);
                }
            }
            tokens.advance(); // consume closing tag
        }
    }
    proceeduralTilesGeneration(1, m_mapSize.x - 1);
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
            m_tileSheetConfig.m_texture = m_context->m_textureManager.acquire(texture_alias);
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

auto Map::proceeduralTilesReGeneration() -> float
{
    const auto [_, Height]    = m_context->m_window.getWindowSize();
    const auto TileSize       = m_tileSheetConfig.m_tileSize;
    const auto [NRows, NCols] = getMapSize();

    const size_t NumberOfRowsInScreen = Height / TileSize;
    ASSERT_DEBUG_BUILD(Height % TileSize == 0, "");

    const size_t NumberOfScreens = NRows / NumberOfRowsInScreen;
    ASSERT_DEBUG_BUILD(NRows % NumberOfRowsInScreen == 0, "");

    size_t i_start_source_row = ((NumberOfScreens - 2) * NumberOfRowsInScreen);
    size_t i_end_source_row   = NRows - 1;

    for (size_t i_source_row = i_start_source_row; i_source_row <= i_end_source_row; i_source_row++)
    {
        size_t dest_i_row = i_source_row - i_start_source_row;
        for (size_t i_col = 0; i_col < NCols; i_col++)
        {
            if (auto it = m_tileMap.find((i_source_row * NCols) + i_col); it != m_tileMap.end())
            {
                m_tileMap.insert_or_assign((dest_i_row * NCols) + i_col, it->second);
            }
            else
            {
                m_tileMap.erase((dest_i_row * NCols) + i_col);
            }
        }
    }

    size_t i_new_start_row = 0 + i_end_source_row - i_start_source_row + 1;

    for (size_t i_row = i_new_start_row; i_row < NRows; i_row++)
    {
        for (size_t i_col = 0; i_col < NCols; i_col++)
        {
            m_tileMap.erase((i_row * NCols) + i_col);
        }
    }

    proceeduralTilesGeneration(i_new_start_row, NRows - 1);

    return ((float)Height * (NumberOfScreens - 2));
}

auto Map::requiresTilesMapRegeneration() -> float
{
    float offset_y = 0.;
    if (requiresProceeduralTilesReGeneration())
    {
        using namespace ecs::messaging;

        LOG("Proceedural tiles re-generation...");

        offset_y = proceeduralTilesReGeneration();

        auto& message_handler = m_context->m_systemManager.getMessageHandler();

        Message msg{.m_type = (MessageType)EntityMessage::Shift_Position,
                    .m_2f   = sf::Vector2f{0., offset_y}};

        message_handler.dispatch(msg);
    }

    return offset_y;
}

void Map::transitionToNextGif()
{
    m_activeGif = m_transitionGif;
}

void Map::update(float l_dt)
{
    sf::FloatRect view_space = core::getViewSpace(m_currentState.getView());
    m_background.setPosition(view_space.left, view_space.top);
    if (m_activeGif != nullptr)
    {
        m_activeGif->setPosition(view_space.getPosition());
        m_activeGif->update(l_dt);
        if (m_activeGif == m_transitionGif && m_activeGif->isDone())
        {
            m_transitionGif->reset();
            size_t id    = m_currentGif - &m_gifs.front();
            id           = (id + 1) % (m_gifs.size() - 1); /*ignore transition gif*/
            m_currentGif = &m_gifs[id];
            m_activeGif  = m_currentGif;
        }
    }
}

void Map::draw()
{
    auto* window = m_context->m_window.getRenderWindow();
    window->draw(m_background);
    if (m_activeGif != nullptr)
    {
        m_activeGif->draw(window);
    }
    const sf::FloatRect ViewSpace = m_context->m_window.getViewSpace();

    float x = ViewSpace.left;
    float y = ViewSpace.top;

    const size_t TileSize = m_tileSheetConfig.m_tileSize;

    int i_row_end = (ViewSpace.top + ViewSpace.height) / m_tileSheetConfig.m_tileSize;
    int i_col_end = (ViewSpace.left + ViewSpace.width) / m_tileSheetConfig.m_tileSize;

    int i_row_begin = std::floor(y / TileSize);
    int i_col_begin = std::ceil(x / TileSize);

    for (int i_row = i_row_begin; i_row <= i_row_end; i_row++)
    {
        for (int i_col = i_col_begin; i_col <= i_col_end; i_col++)
        {
            auto vec = convertCoordinates(i_row, i_col);
            if (!vec.has_value())
            {
                continue;
            }

            auto [i_row_converted, i_col_converted] = vec.value();
            auto tile_index = convertCoordinates(i_row_converted, i_col_converted);
            if (!tile_index.has_value())
            {
                continue;
            }

            if (auto it = m_tileMap.find(tile_index.value()); it != m_tileMap.end())
            {
                auto& tile      = it->second;
                auto& tile_info = *tile.m_tileInfo;
                tile_info.m_sprite.setPosition({(float)i_col * TileSize, (float)i_row * TileSize});
                window->draw(tile_info.m_sprite);
            }
        }
    }
}

auto Map::getTile(size_t iRow, size_t iCol) -> const core::graphics::Tile*
{
    auto tile_index = convertCoordinates(iRow, iCol);

    if (tile_index.has_value())
    {
        if (auto itr = m_tileMap.find(tile_index.value()); itr != m_tileMap.end())
        {
            return &itr->second;
        }
    }

    return nullptr;
}

auto Map::convertCoordinates(int iRow, int iCol) const -> std::optional<sf::Vector2<size_t>>
{
    const auto [_, Height] = m_context->m_window.getWindowSize();
    const auto TileSize    = m_tileSheetConfig.m_tileSize;

    const int NumberOfRowsInScreen = Height / TileSize;
    ASSERT_DEBUG_BUILD(Height % TileSize == 0, "");

    int i_row_converted = -iRow + NumberOfRowsInScreen - 1;
    int i_col_converted = iCol;

    if (i_row_converted < 0 || i_col_converted < 0)
    {
        return {};
    }

    return sf::Vector2<size_t>{(size_t)i_row_converted, (size_t)i_col_converted};
}

auto Map::getTile(int iRow, int iCol) -> const core::graphics::Tile*
{
    if (auto vec = convertCoordinates(iRow, iCol))
    {
        auto [iRowConverted, iColConverted] = vec.value();
        return getTile(iRowConverted, iColConverted);
    }

    return nullptr;
}

auto Map::getTileSheetConfig() const -> const core::graphics::TileSheetConfig&
{
    return m_tileSheetConfig;
}

auto Map::getPlayerId() -> ecs::EntityId
{
    ASSERT(m_playerId.has_value(), "did not load player id");
    return *m_playerId;
}

auto Map::getMapSize() const -> const sf::Vector2u&
{
    return m_mapSize;
}

auto Map::getGravity() const -> float
{
    return m_gravity;
}

void Map::proceeduralTilesGeneration(size_t l_startRow, size_t l_endRow)
{
    if (!m_seeded)
    {
        if (m_seed == 0)
        {
            srand(static_cast<unsigned int>(time(nullptr)));
        }
        else
        {
            srand(m_seed);
        }

        m_seeded = true;
    }

    LOG_DEBUG("Proceedural tiles generation from row = {} to row = {}", l_startRow, l_endRow);

    const auto [_, Height]    = m_context->m_window.getWindowSize();
    const auto TileSize       = m_tileSheetConfig.m_tileSize;
    const auto [NRows, NCols] = getMapSize();

    const size_t NumberOfRowsInScreen = Height / TileSize;
    ASSERT_DEBUG_BUILD(Height % TileSize == 0, "");

    const size_t NumberOfScreens = NRows / NumberOfRowsInScreen;
    ASSERT_DEBUG_BUILD(NRows % NumberOfRowsInScreen == 0, "");

    size_t i_screen_begin     = l_startRow / NumberOfRowsInScreen;
    size_t i_screen_end       = std::min(l_endRow / NumberOfRowsInScreen, NumberOfScreens - 1);
    size_t i_screen_row_start = l_startRow;
    size_t i_screen_row_end   = i_screen_begin == i_screen_end
                                    ? l_endRow
                                    : ((i_screen_begin + 1) * NumberOfRowsInScreen) - 1;

    for (size_t i_screen = i_screen_begin; i_screen <= i_screen_end; ++i_screen)
    {
        long long number_of_tiles_to_fill =
            (i_screen_row_end - i_screen_row_start + 1) * NCols * m_minTilesPercentageOnScreen;
        while (number_of_tiles_to_fill > 0)
        {
            size_t i_row =
                i_screen_row_start + (rand() % (i_screen_row_end - i_screen_row_start + 1));
            size_t i_col  = rand() % NCols;
            size_t length = m_minTilesLength + (rand() % (m_maxTilesLength - m_minTilesLength + 1));
            number_of_tiles_to_fill -= generateTiles(i_row, i_col, length);
        }
        i_screen_row_start = (i_screen + 1) * NumberOfRowsInScreen;
        i_screen_row_end =
            (i_screen + 1 == i_screen_end ? l_endRow : ((i_screen + 2) * NumberOfRowsInScreen) - 1);
    }
}

auto Map::generateTiles(size_t l_row, size_t l_col, size_t l_length) -> size_t
{
    const size_t NCol = m_mapSize.y;

    if (l_length == 1)
    {
        const bool CanHaveTileToItsLeft  = l_col != 0;
        const bool CanHaveTileToItsRight = l_col != NCol - 1;
        if ((CanHaveTileToItsLeft && getTile(l_row, l_col - 1) != nullptr) ||
            (CanHaveTileToItsRight && getTile(l_row, l_col + 1) != nullptr))
        {
            return 0;
        }

        auto [_, added] = m_tileMap.insert_or_assign(*convertCoordinates(l_row, l_col),
                                                     Tile{&m_tileSet.find(ALONE)->second});
        return added ? 1 : 0;
    }

    size_t num_added_tiles = 0;

    for (size_t i = l_col; i < std::min(l_col + l_length, NCol); i++)
    {
        TileId tile_info_id = 0;

        if (i == l_col)
        {
            tile_info_id = LEFT;
        }
        else if (i == l_col + l_length - 1)
        {
            tile_info_id = RIGHT;
        }
        else
        {
            tile_info_id = MIDDLE;
        }

        if (i != l_col || i != l_col + l_length - 1)
        {
            auto [_, added] = m_tileMap.insert_or_assign(
                *convertCoordinates(l_row, i), Tile{&m_tileSet.find(tile_info_id)->second});
            if (added)
            {
                num_added_tiles++;
            }
        }
    }

    return num_added_tiles;
}

auto Map::requiresProceeduralTilesReGeneration() -> bool
{
    const auto [_, Height]    = m_context->m_window.getWindowSize();
    const auto TileSize       = m_tileSheetConfig.m_tileSize;
    const auto [NRows, NCols] = getMapSize();

    const size_t NumberOfRowsInScreen = Height / TileSize;
    ASSERT_DEBUG_BUILD(Height % TileSize == 0, "");

    const size_t NumberOfScreens = NRows / NumberOfRowsInScreen;
    ASSERT_DEBUG_BUILD(NRows % NumberOfRowsInScreen == 0, "");

    int   i_screen_index = -((int)NumberOfScreens - 2);
    float threshold_top  = (float)i_screen_index * (float)Height;

    return core::getViewSpace(m_currentState.getView()).top <= threshold_top;
}