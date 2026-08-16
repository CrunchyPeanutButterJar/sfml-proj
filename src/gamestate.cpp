#include "core/directions.hpp"
#include <core/audio/sound_manager.hpp>
#include <core/bindings.hpp>
#include <core/event_manager.hpp>
#include <core/state/basestate.hpp>
#include <core/state/statemanager.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/messaging/message_handler.hpp>
#include <ecs/shared_context.hpp>
#include <ecs/system/s_collision.hpp>
#include <ecs/system/s_movement.hpp>
#include <ecs/system/system_manager.hpp>
#include <gamestate.hpp>
#include <utils/utilities.hpp>

template class core::RegisterBinding<
    BINDING("Game_MoveRight", core::KeyPressed{sf::Keyboard::Right}), core::Customizable>;
template class core::RegisterBinding<BINDING("Game_MoveLeft", core::KeyPressed{sf::Keyboard::Left}),
                                     core::Customizable>;

template class core::RegisterBinding<BINDING("Game_Jump", core::KeyPressed{sf::Keyboard::Space}),
                                     core::Customizable>;

static void moveEntity(ecs::messaging::MessageHandler& l_messageHandler, ecs::EntityId l_entity,
                       core::Direction l_dir)
{
    using namespace ecs::messaging;

    Message message{.m_type     = (MessageType)EntityMessage::Move,
                    .m_receiver = (int)l_entity,
                    .m_int      = (int)l_dir};

    l_messageHandler.dispatch(message);
}

static void jumpEntity(ecs::messaging::MessageHandler& l_messageHandler, ecs::EntityId l_entity)
{
    ecs::messaging::Message msg{
        .m_type     = (ecs::messaging::MessageType)ecs::messaging::EntityMessage::Jump,
        .m_receiver = (int)l_entity,
        .m_bool     = false /*ignored*/};
    l_messageHandler.dispatch(msg);
}

GameState::GameState(core::state::StateManager& l_stateManager)
    : core::state::BaseState{l_stateManager},
      m_map{m_stateManager.getContext<ecs::SharedContext>(), *this}
{
    m_map.loadMap(utils::getConfigDirectory() + "map.map");

    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    auto* system_manager = &context->m_systemManager;

    auto* s_collision = system_manager->getSystem<ecs::system::SCollision>(ecs::System::Collision);
    auto* s_movement  = system_manager->getSystem<ecs::system::SMovement>(ecs::System::Movement);
    s_collision->setMap(&m_map);
    s_movement->setMap(&m_map);

    auto& sound_manager = context->m_soundManager;

    sound_manager.playMusic("media/audio/game_theme.wav", 70.F, true);

    ecs::EntityId player_id = m_map.getPlayerId();

    auto& event_manager = context->m_eventManager;
    event_manager.addCallback(
        StateType::Game, "Game_MoveRight", [player_id, system_manager](const auto&)
        { moveEntity(system_manager->getMessageHandler(), player_id, core::Direction::Right); });

    event_manager.addCallback(
        StateType::Game, "Game_MoveLeft", [player_id, system_manager](const auto&)
        { moveEntity(system_manager->getMessageHandler(), player_id, core::Direction::Left); });

    event_manager.addCallback(StateType::Game, "Game_Jump", [player_id, system_manager](const auto&)
                              { jumpEntity(system_manager->getMessageHandler(), player_id); });
}

GameState::~GameState()
{
    auto* context       = m_stateManager.getContext<ecs::SharedContext>();
    auto& event_manager = context->m_eventManager;

    event_manager.removeCallback(StateType::Game, "Game_MoveLeft");
    event_manager.removeCallback(StateType::Game, "Game_MoveRight");
    event_manager.removeCallback(StateType::Game, "Game_Jump");
}

void GameState::update(const sf::Time& l_elapsed)
{
    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    m_map.update(l_elapsed.asSeconds());
    context->m_systemManager.update(l_elapsed.asSeconds());
}

void GameState::draw()
{
    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    m_map.draw();
    context->m_systemManager.draw(context->m_window);
}
