#include <utils/utilities.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <gamestate.hpp>

#include <core/eventmanager.hpp>
#include <ecs/entity/entitymanager.hpp>
#include <ecs/messaging/message.hpp>
#include <ecs/messaging/entitymessage.hpp>
#include <ecs/system/systemmanager.hpp>
#include <statemanager.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/c_spritesheet.hpp>
#include <core/window.hpp>

#include <SFML/Window/Keyboard.hpp>

GameState::GameState(StateManager& l_stateManager):
BaseState(l_stateManager),
m_gameMap{m_stateManager.getContext(), *this}
{
    m_gameMap.loadMap(utils::getConfigDirectory() + "map.map");

    auto& event_manager = m_stateManager.getContext().m_eventManager;
    auto& entity_manager = m_stateManager.getContext().m_entityManager;
    auto& message_handler = m_stateManager.getContext().m_systemManager.getMessageHandler();

    ecs::EntityId player_id = m_gameMap.getPlayerId(); 

    auto* sprite = entity_manager.getComponent<ecs::entity::CSpriteSheet>(player_id, ecs::Component::SpriteSheet);
    sprite->getSpriteSheet()->nextAnimation();

    event_manager.addCallback((core::EventManager::StateType)StateType::Game, "Mouse_Moved",
    [&entity_manager, player_id](const sf::Window::WindowBase& l_window)
    {
        auto* position = entity_manager.getComponent<ecs::entity::CPosition>(player_id, ecs::Component::Position);
        auto [ix, iy] = sf::Mouse::getPosition(l_window);
        position->setPosition({(float)ix, (float)iy});
    });

    event_manager.addCallback((core::EventManager::StateType)StateType::Game, "Game_MoveLeft",
    [player_id, &message_handler](const sf::WindowBase&)
    {
        static bool move_left = true;
        ecs::messaging::Message msg{.m_type = (ecs::messaging::MessageType)ecs::messaging::EntityMessage::Direction_Changed, .m_receiver = (int) player_id, .m_int = (int)(move_left ? core::Direction::Left : core::Direction::Right)};
        message_handler.dispatch(msg);
        move_left = !move_left;
    });
}

GameState::~GameState() = default;

void GameState::update(const sf::Time& l_elapsed)
{
    m_elapsed += l_elapsed;

    m_gameMap.update(l_elapsed.asSeconds());
    m_stateManager.getContext().m_systemManager.update(l_elapsed.asSeconds());

    float timestep = 1.0F / 60;

    if(m_elapsed.asSeconds() >= timestep)
    {
        m_elapsed -= sf::seconds(timestep);
    }
}

void GameState::draw()
{
    auto& system_manager = m_stateManager.getContext().m_systemManager;
    auto& window = m_stateManager.getContext().m_window;
    
    m_gameMap.draw();
    system_manager.draw(window);
}

void GameState::activate()
{
}

void GameState::deactivate()
{
}

void GameState::onCreate()
{
}