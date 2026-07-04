#include "utils/utilities.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <gamestate.hpp>

#include <eventmanager.hpp>
#include <entitymanager.hpp>
#include <message.hpp>
#include <entitymessage.hpp>
#include <systemmanager.hpp>
#include <statemanager.hpp>
#include <c_position.hpp>
#include <c_spritesheet.hpp>
#include <window.hpp>

#include <SFML/Window/Keyboard.hpp>

GameState::GameState(StateManager& l_stateManager):
BaseState(l_stateManager),
m_gameMap{m_stateManager.GetContext(), *this}
{
    m_gameMap.loadMap(Utils::GetConfigDirectory() + "map.map");

    auto& eventManager = m_stateManager.GetContext().m_eventManager;
    auto& entityManager = m_stateManager.GetContext().m_entityManager;
    auto& messageHandler = m_stateManager.GetContext().m_systemManager.getMessageHandler();

    EntityId playerId = m_gameMap.getPlayerId(); 

    auto* sprite = entityManager.getComponent<C_SpriteSheet>(playerId, Component::SpriteSheet);
    sprite->getSpriteSheet()->nextAnimation();

    eventManager.AddCallback(StateType::Game, "Mouse_Moved",
    [&entityManager, playerId](const sf::Window::WindowBase& l_window)
    {
        auto* position = entityManager.getComponent<C_Position>(playerId, Component::Position);
        auto [ix, iy] = sf::Mouse::getPosition(l_window);
        position->setPosition({(float)ix, (float)iy});
    });

    eventManager.AddCallback(StateType::Game, "Game_MoveLeft",
    [playerId, &messageHandler](const sf::WindowBase&)
    {
        static bool moveLeft = true;
        Message msg{.m_type = (MessageType)EntityMessage::Direction_Changed, .m_receiver = (int) playerId, .m_int = (int)(moveLeft ? Direction::Left : Direction::Right)};
        messageHandler.dispatch(msg);
        moveLeft = !moveLeft;
    });
}

GameState::~GameState()
{
}

void GameState::Update(const sf::Time& l_elapsed)
{
    m_elapsed += l_elapsed;

    m_gameMap.update(l_elapsed.asSeconds());
    m_stateManager.GetContext().m_systemManager.update(l_elapsed.asSeconds());

    float timestep = 1.0f / 60;

    if(m_elapsed.asSeconds() >= timestep)
    {
        m_elapsed -= sf::seconds(timestep);
    }
}

void GameState::Draw()
{
    auto& systemManager = m_stateManager.GetContext().m_systemManager;
    auto& window = m_stateManager.GetContext().m_window;
    
    m_gameMap.draw();
    systemManager.draw(window);
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