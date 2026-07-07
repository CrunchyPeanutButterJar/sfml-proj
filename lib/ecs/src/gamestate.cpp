#include "core/directions.hpp"
#include "ecs/ecs_types.hpp"
#include "ecs/messaging/message_handler.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <ecs/state/gamestate.hpp>
#include <utils/utilities.hpp>

#include <core/event_manager.hpp>
#include <core/window.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/c_spritesheet.hpp>
#include <ecs/messaging/entity_message.hpp>
#include <ecs/messaging/message.hpp>
#include <ecs/state/statemanager.hpp>
#include <ecs/system/s_movement.hpp>
#include <ecs/system/system_manager.hpp>

#include <SFML/Window/Keyboard.hpp>

using namespace ecs::state;

static void moveEntity(ecs::messaging::MessageHandler& l_messageHandler, ecs::EntityId l_entity,
                       core::Direction l_dir)
{
    using namespace ecs::messaging;

    Message message{.m_type     = (MessageType)EntityMessage::Move,
                    .m_receiver = (int)l_entity,
                    .m_int      = (int)l_dir};

    l_messageHandler.dispatch(message);
}

GameState::GameState(StateManager& l_stateManager)
    : BaseState(l_stateManager), m_gameMap{m_stateManager.getContext(), *this}
{
    m_gameMap.loadMap(utils::getConfigDirectory() + "map.map");

    auto& event_manager   = m_stateManager.getContext().m_eventManager;
    auto& system_manager  = m_stateManager.getContext().m_systemManager;
    auto& message_handler = m_stateManager.getContext().m_systemManager.getMessageHandler();

    system_manager.getSystem<ecs::system::SMovement>(System::Movement)->setMap(&m_gameMap);

    ecs::EntityId player_id = m_gameMap.getPlayerId();

    event_manager.addCallback((core::EventManager::StateType)StateType::Game, "Game_MoveLeft",
                              [player_id, &message_handler](const sf::WindowBase&)
                              { moveEntity(message_handler, player_id, core::Direction::Left); });

    event_manager.addCallback((core::EventManager::StateType)StateType::Game, "Game_MoveRight",
                              [player_id, &message_handler](const sf::WindowBase&)
                              { moveEntity(message_handler, player_id, core::Direction::Right); });
}

GameState::~GameState() = default;

void GameState::update(const sf::Time& l_elapsed)
{
    m_elapsed += l_elapsed;

    m_gameMap.update(l_elapsed.asSeconds());
    m_stateManager.getContext().m_systemManager.update(l_elapsed.asSeconds());

    float timestep = 1.0F / 60;

    if (m_elapsed.asSeconds() >= timestep)
    {
        m_elapsed -= sf::seconds(timestep);
    }
}

void GameState::draw()
{
    auto& system_manager = m_stateManager.getContext().m_systemManager;
    auto& window         = m_stateManager.getContext().m_window;

    m_gameMap.draw();
    system_manager.draw(window);
}

void GameState::activate() {}

void GameState::deactivate() {}

void GameState::onCreate() {}