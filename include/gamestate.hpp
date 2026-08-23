#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include "ecs/map.hpp"
#include <statetype.hpp>

#include <core/state/basestate.hpp>
#include <enemy_entities_manager.hpp>

class GameState : public core::state::BaseState
{
  public:
    GameState(core::state::StateManager& l_stateManager);
    ~GameState() override;

    void update(const sf::Time& l_elapsed) override;
    void draw() override;

    void activate() override {};
    void deactivate() override {};

    void onCreate() override {};

    void updateScore();
    auto getScore()->unsigned int;

    static constexpr StateType TYPE = StateType::Game;

  private:
    void updateCamera(const sf::Time& l_elapsed);
    auto playerHasLost() -> bool;

  
    ecs::Map m_map;
    EnemyEntitiesManager m_enemyEntitiesManager;
    float m_offsetY{0.F};
    bool m_playerHasJumped{};
};

#endif