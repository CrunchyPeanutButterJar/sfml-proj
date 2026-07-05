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
    virtual ~GameState();

    void Update(const sf::Time& l_elapsed) override;
    void Draw() override;

    void Activate() override;
    void Deactivate() override;

    void OnCreate() override;

private:
    sf::Time m_elapsed;
    Map m_gameMap;
};

#endif