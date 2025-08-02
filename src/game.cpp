#include <game.hpp>

#include <assert.hpp>

#include <fstream>
#include <rfl/AddTagsToVariants.hpp>
#include <sstream>
#include <string>

#include <rfl.hpp>
#include <rfl/json.hpp>

using Resolution = std::pair<uint32_t, uint32_t>;

static sf::Vector2u loadResolutionFromConfigFile()
{
    static const std::string ConfigFileName = "config.json";

    std::ifstream config{ConfigFileName};
    if(config.good())
    {
        std::stringstream buffer;
        buffer << config.rdbuf();

        std::string jsonConfig = buffer.str();
        
        if(auto result = rfl::json::read<Resolution, rfl::AddTagsToVariants>(jsonConfig))
        {
            auto resolution = result.value();
            LOG("Loaded Config with width = {} and height = {}", resolution.first, resolution.second);

            return {resolution.first, resolution.second};
        }
        FAILURE("Error while parsing invalid file : {}", ConfigFileName);
    }
    else
    {
        const Resolution defaultResolution{800, 600};

        std::ofstream config{ConfigFileName};
        LOG_ERROR(config.good(), "Error while writing to file : {}", ConfigFileName);
        config << rfl::json::write<rfl::AddTagsToVariants>(defaultResolution);
        
        LOG("Loaded default Config with width = {} and height = {}", defaultResolution.first, defaultResolution.second);
        return {defaultResolution.first, defaultResolution.second};
    }
} 

Game::Game() :
m_window{"untitled", loadResolutionFromConfigFile()},
m_stateManager{std::make_tuple(std::ref(m_window), std::ref(m_window.GetEventManager()))}
{
    m_stateManager.SwitchTo(StateType::Game);
}

void Game::Update()
{
    m_window.Update(m_stateManager.GetCurrentState());
    m_stateManager.Update(m_elapsed);
}

void Game::LateUpdate()
{
    m_stateManager.ProcessRequests();
    RestartClock();
}

sf::Time Game::GetElapsed()
{
    return m_elapsed;
}

void Game::RestartClock()
{
    m_elapsed = m_clock.restart();
}

Window* Game::GetWindow()
{
    return &m_window;
}

void Game::Render()
{
    m_window.BeginDraw();
    m_stateManager.Draw();
    m_window.EndDraw();
}