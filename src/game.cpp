#include <game.hpp>

#include <utils/assert.hpp>
#include <utils/utilities.hpp>

#include <fstream>
#include <rfl/AddTagsToVariants.hpp>
#include <sstream>
#include <string>

#include <rfl.hpp>
#include <rfl/json.hpp>

using Resolution = std::pair<uint32_t, uint32_t>;

static sf::Vector2u loadResolutionFromConfigFile()
{
    static const std::string ConfigFileName = Utils::getConfigDirectory() + "config.json";

    std::ifstream config{ConfigFileName};
    if(config.good())
    {
        std::stringstream buffer;
        buffer << config.rdbuf();

        std::string json_config = buffer.str();
        
        if(auto result = rfl::json::read<Resolution, rfl::AddTagsToVariants>(json_config))
        {
            auto resolution = result.value();
            LOG("Loaded Config file {} with width = {} and height = {}", ConfigFileName, resolution.first, resolution.second);

            return {resolution.first, resolution.second};
        }
        FAILURE("Error while parsing invalid file : {}", ConfigFileName);
        return {};
    }
    else
    {
        const Resolution defaultResolution{800, 600};

        std::ofstream config{ConfigFileName};
        ASSERT_NON_FATAL(config.good(), "Error while writing to file : {}", ConfigFileName);
        config << rfl::json::write<rfl::AddTagsToVariants>(defaultResolution);
        
        LOG("Loaded default Config with width = {} and height = {}", defaultResolution.first, defaultResolution.second);
        return {defaultResolution.first, defaultResolution.second};
    }
} 

Game::Game() :
m_window{"MyGame", loadResolutionFromConfigFile()},
m_stateManager{{m_window, m_window.getEventManager(), m_entityManager, m_systemManager, m_textureManager}},
m_entityManager{m_systemManager, m_stateManager.getContext().m_textureManager},
m_systemManager{m_entityManager}
{
        m_stateManager.switchTo(StateType::Game);
}

void Game::update()
{
    m_window.update((EventManager::StateType)m_stateManager.getCurrentState());
    m_stateManager.update(m_elapsed);
}

void Game::lateUpdate()
{
    m_stateManager.processRequests();
    restartClock();
}

sf::Time Game::getElapsed()
{
    return m_elapsed;
}

void Game::restartClock()
{
    m_elapsed = m_clock.restart();
}

Window* Game::getWindow()
{
    return &m_window;
}

void Game::render()
{
    m_window.beginDraw();
    m_stateManager.draw();
    m_window.endDraw();
}