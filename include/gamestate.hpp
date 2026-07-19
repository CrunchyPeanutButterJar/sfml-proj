#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <SFML/System/Time.hpp>
#include <statetype.hpp>

#include <core/graphics/spritesheet.hpp>
#include <ecs/map.hpp>
#include <core/state/basestate.hpp>

class GameState : public core::state::BaseState
{
  public:
    GameState(core::state::StateManager& l_stateManager);
    ~GameState() override;

    void update(const sf::Time& l_elapsed) override;
    void draw() override;

    void activate() override;
    void deactivate() override;

    void onCreate() override;

    static constexpr StateType TYPE = StateType::Game;

  private:
    sf::Time m_elapsed;
    ecs::Map      m_gameMap;
};

#endif