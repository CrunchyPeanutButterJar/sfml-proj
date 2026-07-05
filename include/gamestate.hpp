#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <SFML/System/Time.hpp>

#include <map.hpp>
#include <basestate.hpp>
#include <core/graphics/spriteSheet.hpp>

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
    Map m_gameMap;
};

#endif