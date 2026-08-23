#include "ecs/ecs_types.hpp"
#include <SFML/System/Vector2.hpp>
#include <core/window.hpp>
#include <ecs/entity/c_aicontroller.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/map.hpp>
#include <enemy_entities_manager.hpp>
#include <sstream>
#include <utils/assert.hpp>
#include <utils/random.hpp>
#include <utils/utilities.hpp>

EnemyEntitiesManager::EnemyEntitiesManager(ecs::entity::EntityManager& l_entityManager,
                                           const ecs::Map&             l_map)
    : m_entityManager{l_entityManager}, m_map{l_map}
{
    constexpr std::string ConfigFile = "enemies.cfg";

    auto file = utils::readFile(utils::getConfigDirectory() + ConfigFile);
    if (!file.has_value())
    {
        FAILURE_NON_FATAL("Could not load {}", ConfigFile);
        return;
    }

    utils::Tokens tokens{std::istringstream{std::move(file.value())}};

    while (!tokens.empty())
    {
        auto key = *consumeToken<std::string>(tokens);
        if (key == "NumberOfEnemiesOnScreen")
        {
            auto [min, max]   = *consumeTokens<unsigned int, unsigned int>(tokens);
            m_enemySpawnRange = std::make_pair(min, max);
        }
        else if (key == "TraversalLength")
        {
            auto [min, max]       = *consumeTokens<float, float>(tokens);
            m_enemyTraversalRange = std::make_pair(min, max);
        }
    }
}

static auto generateRandomPosition(int l_screen_index, unsigned int screen_width,
                                   unsigned int screen_height) -> sf::Vector2f
{
    static constexpr float OFFSET = 16.F;

    float min_x = OFFSET;
    float max_x = (float)screen_width - OFFSET;

    float min_y = ((float)l_screen_index * (float)screen_height) + OFFSET;
    float max_y = ((float)(l_screen_index + 1) * (float)screen_height) - OFFSET;

    return {utils::getRandom(min_x, max_x), utils::getRandom(min_y, max_y)};
}

void EnemyEntitiesManager::generateEnemies()
{
    const auto NumberOfScreens = m_map.getNumberOfScreens();
    const auto [Width, Height] = m_map.getContext()->m_window.getWindowSize();

    const int i_ScreenEnd   = -((int)NumberOfScreens - 2);
    const int i_ScreenStart = -1;

    for (int i_screen = i_ScreenStart; i_screen >= i_ScreenEnd; i_screen--)
    {
        auto enemies_to_generate =
            utils::getRandom<unsigned int>(m_enemySpawnRange.first, m_enemySpawnRange.second);

        auto max_x = (float)Width;
        auto min_x = 0.F;

        auto min_y = ((float)i_screen * (float)Height);
        auto max_y = ((float)(i_screen + 1) * (float)Height);

        while (enemies_to_generate-- > 0)
        {
            auto pos       = generateRandomPosition(i_screen, Width, Height);
            auto new_enemy = addDemon(pos);
            if (new_enemy)
            {
                constexpr auto GetRandomTraversalLength = [](auto l_range) -> auto
                { return utils::getRandom(l_range.first, l_range.second); };

                auto new_goal = pos;
                if (utils::getRandom(0, 1) == 1)
                {
                    if (utils::getRandom(0, 1) == 1)
                    {
                        new_goal.x = std::min(
                            new_goal.x + GetRandomTraversalLength(m_enemyTraversalRange), max_x);
                    }
                    else
                    {
                        new_goal.x = std::max(
                            new_goal.x - GetRandomTraversalLength(m_enemyTraversalRange), min_x);
                    }
                }
                else
                {
                    if (utils::getRandom(0, 1) == 1)
                    {
                        new_goal.y = std::min(
                            new_goal.y + GetRandomTraversalLength(m_enemyTraversalRange), max_y);
                    }
                    else
                    {
                        new_goal.y = std::max(
                            new_goal.y - GetRandomTraversalLength(m_enemyTraversalRange), min_y);
                    }
                }

                m_entityManager
                    .getComponent<ecs::entity::CAIController>(new_enemy.value(),
                                                              ecs::Component::AIController)
                    ->setGoals({new_goal, pos});
            }
        }
    }
}

auto EnemyEntitiesManager::addDemon(const sf::Vector2f& l_pos) -> std::optional<ecs::EntityId>
{
    auto new_enemy = m_entityManager.addEntity("demon.entity");
    if (new_enemy < 0)
    {
        FAILURE_NON_FATAL("Failed to add enemy");
        return {};
    }

    m_entityManager.getComponent<ecs::entity::CPosition>(new_enemy, ecs::Component::Position)
        ->setPosition(l_pos);

    m_enemies.push_back(new_enemy);
    return new_enemy;
}

EnemyEntitiesManager::~EnemyEntitiesManager()
{
    for (auto enemy : m_enemies)
    {
        m_entityManager.removeEntity(enemy);
    }
}
