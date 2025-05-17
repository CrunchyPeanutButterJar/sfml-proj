#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include <snake.hpp>
#include <textbox.hpp>
#include <window.hpp>
#include <world.hpp>

class Game
{
public:
    Game();
    ~Game() = default;

    void HandleInput();
    void Update();
    void Render();
    Window* GetWindow();
    sf::Time GetElapsed();
    void RestartClock();

private:
    sf::Clock m_clock;
    sf::Time m_elapsed;

    Window m_window;
    World m_world;
    Snake m_snake;
    Textbox m_textbox;
};

#endif