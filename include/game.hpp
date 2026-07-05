#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include <ecs/entity/entity_manager.hpp>
#include <ecs/system/system_manager.hpp>
#include <ecs/state/statemanager.hpp>
#include <core/window.hpp>

class Game
{
public:
    Game();
    ~Game() = default;

    void update();
    void lateUpdate();
    void render();
    auto getWindow() -> core::Window*;
    auto getElapsed() -> sf::Time;

private:
    void restartClock();


    sf::Clock m_clock;
    sf::Time m_elapsed;

    core::Window m_window;
    ecs::state::StateManager m_stateManager;
    ecs::entity::EntityManager m_entityManager;
    ecs::system::SystemManager m_systemManager;
    core::graphics::TextureManager m_textureManager;
};

#endif