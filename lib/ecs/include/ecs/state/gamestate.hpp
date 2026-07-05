#ifndef ECS_STATE_GAMESTATE_HPP
#define ECS_STATE_GAMESTATE_HPP

#include <SFML/System/Time.hpp>

#include <core/graphics/spritesheet.hpp>
#include <ecs/map.hpp>
#include <ecs/state/basestate.hpp>

namespace ecs::state
{
class GameState : public BaseState
{
  public:
    GameState(StateManager& l_stateManager);
    ~GameState() override;

    void update(const sf::Time& l_elapsed) override;
    void draw() override;

    void activate() override;
    void deactivate() override;

    void onCreate() override;

  private:
    sf::Time m_elapsed;
    Map      m_gameMap;
};
} // namespace ecs::state

#endif