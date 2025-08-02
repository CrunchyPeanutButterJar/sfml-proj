#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <SFML/System/Time.hpp>

#include <basestate.hpp>
#include <world.hpp>

class GameState : public BaseState
{
public:
    GameState(StateManager& l_stateManager);
    virtual ~GameState();

    void Update(const sf::Time& l_elapsed) override;
    void Draw() override;

    void Activate() override;
    void Deactivate() override;

private:
    World m_world;
    sf::Time m_elapsed;
};

#endif