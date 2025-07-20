#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include <statemanager.hpp>
#include <window.hpp>

class Game
{
public:
    Game();
    ~Game() = default;

    void Update();
    void LateUpdate();
    void Render();
    Window* GetWindow();
    sf::Time GetElapsed();

private:
    void RestartClock();

private:
    sf::Clock m_clock;
    sf::Time m_elapsed;

    Window m_window;
    StateManager m_stateManager;
};

#endif