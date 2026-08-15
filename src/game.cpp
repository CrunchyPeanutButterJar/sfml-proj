#include "core/gui/GUI_event.hpp"
#include "ecs/ecs_types.hpp"
#include <SFML/Graphics/View.hpp>
#include <game.hpp>

#include <ecs/system/s_sound.hpp>
#include <mainmenustate.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

#include <fstream>
#include <rfl/AddTagsToVariants.hpp>
#include <sstream>
#include <string>

#include <rfl.hpp>
#include <rfl/json.hpp>

using Resolution = std::pair<uint32_t, uint32_t>;

static auto loadResolutionFromConfigFile() -> sf::Vector2u
{
    static const std::string ConfigFileName = utils::getConfigDirectory() + "config.json";

    std::ifstream config{ConfigFileName};
    if (config.good())
    {
        std::stringstream buffer;
        buffer << config.rdbuf();

        std::string json_config = buffer.str();

        if (auto result = rfl::json::read<Resolution, rfl::AddTagsToVariants>(json_config))
        {
            auto resolution = result.value();
            LOG("Loaded Config file {} with width = {} and height = {}", ConfigFileName,
                resolution.first, resolution.second);

            return {resolution.first, resolution.second};
        }
        FAILURE("Error while parsing invalid file : {}", ConfigFileName);
        return {};
    }

    {
        const Resolution DefaultResolution{800, 600};

        std::ofstream config{ConfigFileName};
        ASSERT_NON_FATAL(config.good(), "Error while writing to file : {}", ConfigFileName);
        config << rfl::json::write<rfl::AddTagsToVariants>(DefaultResolution);

        LOG("Loaded default Config with width = {} and height = {}", DefaultResolution.first,
            DefaultResolution.second);
        return {DefaultResolution.first, DefaultResolution.second};
    }
}

static const sf::Vector2u INGAME_RESOLUTION = {320, 320};

Game::Game()
    : m_window{"MyGame", INGAME_RESOLUTION, loadResolutionFromConfigFile()},
      m_context{ecs::SharedContextBuilder::build({.m_window         = m_window,
                                                  .m_eventManager   = m_window.getEventManager(),
                                                  .m_textureManager = m_textureManager,
                                                  .m_fontManager    = m_fontManager,
                                                  .m_guiManager     = m_guiManager,
                                                  .m_audioManager   = m_audioManager,
                                                  .m_soundManager   = m_soundManager,
                                                  .m_entityManager  = m_entityManager,
                                                  .m_systemManager  = m_systemManager})},
      m_stateManager{core::state::StateManager::build<MainMenuState>(m_context)},
      m_entityManager{m_systemManager, m_stateManager.getContext()->m_textureManager},
      m_systemManager{m_entityManager},
      m_guiManager(&m_window.getEventManager(), m_stateManager.getContext()),
      m_soundManager{m_audioManager}
{
    m_stateManager.switchTo(StateType::MainMenu);

    m_systemManager.getSystem<ecs::system::SSound>(ecs::System::Sound)
        ->setUp(&m_audioManager, &m_soundManager);
}

void Game::update()
{
    m_window.update(m_stateManager.getCurrentState());
    m_stateManager.update(m_elapsed);
    m_guiManager.update(m_elapsed.asSeconds());
    m_soundManager.update(m_elapsed.asSeconds());
    core::gui::GUI_Event gui_event{};
    while (m_guiManager.pollEvent(gui_event))
    {
        m_context.m_eventManager.handleEvent(gui_event);
    }
}

void Game::lateUpdate()
{
    m_stateManager.processRequests();
    restartClock();
}

auto Game::getElapsed() -> sf::Time
{
    return m_elapsed;
}

void Game::restartClock()
{
    m_elapsed = m_clock.restart();
}

auto Game::getWindow() -> core::Window*
{
    return &m_window;
}

void Game::render()
{
    m_window.beginDraw();
    m_stateManager.draw();

    auto current_view = m_window.getRenderWindow()->getView();
    m_window.getRenderWindow()->setView(m_window.getRenderWindow()->getDefaultView());
    m_guiManager.render(m_window.getRenderWindow());
    m_window.getRenderWindow()->setView(current_view);

    m_window.endDraw();
}