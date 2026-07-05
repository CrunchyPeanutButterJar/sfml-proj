#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include <ecs/entity/entitymanager.hpp>
#include <ecs/system/systemmanager.hpp>
#include <statemanager.hpp>
#include <core/window.hpp>

class Game
{
public:
    Game();
    ~Game() = default;

    void update();
    void lateUpdate();
    void render();
    Window* getWindow();
    sf::Time getElapsed();

private:
    void restartClock();


    sf::Clock m_clock;
    sf::Time m_elapsed;

    Window m_window;
    StateManager m_stateManager;
    EntityManager m_entityManager;
    SystemManager m_systemManager; // to handle cyclic dependency systemmanager <-> entityManager
    TextureManager m_textureManager;
};

#endif