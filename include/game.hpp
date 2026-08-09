#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include <ecs/entity/entity_manager.hpp>
#include <ecs/system/system_manager.hpp>
#include <core/state/statemanager.hpp>
#include <ecs/shared_context.hpp>
#include <core/window.hpp>
#include <core/gui/GUI_manager.hpp>
#include <core/font_manager.hpp>
#include <core/audio/audio_manager.hpp>
#include <core/audio/sound_manager.hpp>

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

    ecs::SharedContext m_context;

    core::state::StateManager m_stateManager;
    ecs::entity::EntityManager m_entityManager;
    ecs::system::SystemManager m_systemManager;
    core::graphics::TextureManager m_textureManager;
    core::FontManager m_fontManager;
    core::gui::GUI_Manager m_guiManager;
    core::audio::AudioManager m_audioManager;
    core::audio::SoundManager m_soundManager;

};

#endif