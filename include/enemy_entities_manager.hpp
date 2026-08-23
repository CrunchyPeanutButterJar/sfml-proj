#ifndef ENEMY_ENTITIES_MANAGER_HPP
#define ENEMY_ENTITIES_MANAGER_HPP

#include <SFML/System/Vector2.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/entity/entity_manager.fwd.hpp>
#include <ecs/map.fwd.hpp>
#include <optional>
#include <vector>

class EnemyEntitiesManager
{
  public:
    EnemyEntitiesManager(ecs::entity::EntityManager& l_entityManager, const ecs::Map& l_map);
    ~EnemyEntitiesManager();

    void generateEnemies();

  private:
    auto addDemon(const sf::Vector2f& l_pos) -> std::optional<ecs::EntityId>;

  
    ecs::entity::EntityManager&     m_entityManager;
    const ecs::Map&                 m_map;
    std::vector<ecs::EntityId>      m_enemies;
    std::pair<uint8_t, uint8_t>     m_enemySpawnRange;
    std::pair<float, float>   m_enemyTraversalRange;
};

#endif