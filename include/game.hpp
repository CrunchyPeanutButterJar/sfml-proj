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
    core::Window* getWindow();
    sf::Time getElapsed();

private:
    void restartClock();


    sf::Clock m_clock;
    sf::Time m_elapsed;

    core::Window m_window;
    StateManager m_stateManager;
    ecs::entity::EntityManager m_entityManager;
    ecs::system::SystemManager m_systemManager;
    core::graphics::TextureManager m_textureManager;
};

#endif