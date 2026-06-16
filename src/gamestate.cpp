//#include <utilities/utilities.hpp>
#include <SFML/System/Time.hpp>
#include <gamestate.hpp>

#include <eventmanager.hpp>
#include <statemanager.hpp>
#include <window.hpp>

#include <SFML/Window/Keyboard.hpp>

static SpriteSheet loadSpriteSheet(TextureManager& l_textureManager)
{
    SpriteSheet sprite{l_textureManager};

    ASSERT(sprite.loadSheet(Utils::GetConfigDirectory()+"samuraiSprite.sheet"), "Could not load sprite sheet");

    return sprite;
}

GameState::GameState(StateManager& l_stateManager): BaseState(l_stateManager), m_sprite(loadSpriteSheet(l_stateManager.GetContext().m_textureManager))
{
    auto& eventManager = m_stateManager.GetContext().m_eventManager;
    auto& sprite = this->m_sprite;

    eventManager.AddCallback(StateType::Game, "Game_MoveRight", [&sprite](const auto&) -> void {sprite.nextAnimation();});
}

GameState::~GameState()
{
}

void GameState::Update(const sf::Time& l_elapsed)
{
    m_elapsed += l_elapsed;

    m_sprite.update(l_elapsed.asSeconds());

    float timestep = 1.0f / 60;

    if(m_elapsed.asSeconds() >= timestep)
    {
        m_elapsed -= sf::seconds(timestep);
    }
}

void GameState::Draw()
{
    m_sprite.draw(m_stateManager.GetContext().m_window.GetRenderWindow());
}

void GameState::Activate()
{
}

void GameState::Deactivate()
{
}

void GameState::OnCreate()
{
}