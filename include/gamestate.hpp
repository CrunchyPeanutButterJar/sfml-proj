#ifndef GAMESTATE_HPP
#define GAMESTATE_HPP

#include <SFML/System/Time.hpp>

#include <basestate.hpp>
#include <snake.hpp>
#include <textbox.hpp>
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
    Textbox m_textbox;
    World m_world;
    Snake m_snake;
    sf::Time m_elapsed;
};

#endif