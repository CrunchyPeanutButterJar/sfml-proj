#include "core/directions.hpp"
#include "ecs/ecs_types.hpp"
#include "ecs/messaging/message_handler.hpp"
#include "ecs/shared_context.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <core/state/statemanager.hpp>
#include <gamestate.hpp>
#include <utils/utilities.hpp>

#include <core/bindings.hpp>
#include <core/event_manager.hpp>
#include <core/window.hpp>
#include <ecs/entity/c_collidable.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/c_spritesheet.hpp>
#include <ecs/messaging/entity_message.hpp>
#include <ecs/messaging/message.hpp>
#include <ecs/system/s_collision.hpp>
#include <ecs/system/s_movement.hpp>
#include <ecs/system/system_manager.hpp>

#include <SFML/Window/Keyboard.hpp>

using namespace ecs;

static void moveEntity(ecs::messaging::MessageHandler& l_messageHandler, ecs::EntityId l_entity,
                       core::Direction l_dir)
{
    using namespace ecs::messaging;

    Message message{.m_type     = (MessageType)EntityMessage::Move,
                    .m_receiver = (int)l_entity,
                    .m_int      = (int)l_dir};

    l_messageHandler.dispatch(message);
}

template class core::RegisterBinding<BINDING("Game_ToggleSpriteSheetOverlay",
                                             core::KeyPressed{sf::Keyboard::O}),
                                     core::Customizable>;
template class core::RegisterBinding<BINDING("Game_ToggleCollidableDebugOverlay",
                                             core::KeyPressed{sf::Keyboard::P}),
                                     core::Customizable>;

template class core::RegisterBinding<BINDING("Game_MoveUp", core::KeyPressed{sf::Keyboard::Up}),
                                     core::Customizable>;
template class core::RegisterBinding<BINDING("Game_MoveDown", core::KeyPressed{sf::Keyboard::Down}),
                                     core::Customizable>;
template class core::RegisterBinding<BINDING("Game_MoveLeft", core::KeyPressed{sf::Keyboard::Left}),
                                     core::Customizable>;
template class core::RegisterBinding<
    BINDING("Game_MoveRight", core::KeyPressed{sf::Keyboard::Right}), core::Customizable>;

template class core::RegisterBinding<BINDING("Game_MoveUp", core::KeyPressed{sf::Keyboard::W}),
                                     core::Customizable>;
template class core::RegisterBinding<BINDING("Game_MoveDown", core::KeyPressed{sf::Keyboard::S}),
                                     core::Customizable>;
template class core::RegisterBinding<BINDING("Game_MoveLeft", core::KeyPressed{sf::Keyboard::A}),
                                     core::Customizable>;
template class core::RegisterBinding<BINDING("Game_MoveRight", core::KeyPressed{sf::Keyboard::D}),
                                     core::Customizable>;

template class core::RegisterBinding<BINDING("Game_Jump", core::KeyPressed{sf::Keyboard::Space}),
                                     core::Customizable>;

GameState::GameState(core::state::StateManager& l_stateManager)
    : BaseState(l_stateManager), m_gameMap{m_stateManager.getContext<ecs::SharedContext>(), *this}
{
    m_gameMap.loadMap(utils::getConfigDirectory() + "map.map");

    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    auto& event_manager   = context->m_eventManager;
    auto& system_manager  = context->m_systemManager;
    auto& message_handler = context->m_systemManager.getMessageHandler();

    system_manager.getSystem<ecs::system::SMovement>(System::Movement)->setMap(&m_gameMap);
    system_manager.getSystem<ecs::system::SCollision>(System::Collision)->setMap(&m_gameMap);

    ecs::EntityId player_id = m_gameMap.getPlayerId();

    event_manager.addCallback(StateType::Game, "Game_MoveLeft",
                              [player_id, &message_handler](const auto&)
                              { moveEntity(message_handler, player_id, core::Direction::Left); });

    event_manager.addCallback(StateType::Game, "Game_MoveRight",
                              [player_id, &message_handler](const auto&)
                              { moveEntity(message_handler, player_id, core::Direction::Right); });

    event_manager.addCallback(
        StateType::Game, "Game_Jump",
        [player_id, &message_handler](const auto&)
        {
            ecs::messaging::Message msg{
                .m_type     = (ecs::messaging::MessageType)ecs::messaging::EntityMessage::Jump,
                .m_receiver = (int)player_id,
                .m_bool     = false /*ignored*/};
            message_handler.dispatch(msg);
        });

    event_manager.addCallback(StateType::Game, "Game_ToggleSpriteSheetOverlay",
                              [](const auto& l_details)
                              {
                                  if (!l_details.m_realtimeContribution)
                                  {
                                      ecs::entity::CSpriteSheet::debug_overlay =
                                          !ecs::entity::CSpriteSheet::debug_overlay;
                                  }
                              });

    event_manager.addCallback(StateType::Game, "Game_ToggleCollidableDebugOverlay",
                              [](const auto& l_details)
                              {
                                  if (!l_details.m_realtimeContribution)
                                  {
                                      ecs::entity::CCollidable::debug_overlay =
                                          !ecs::entity::CCollidable::debug_overlay;
                                  }
                              });
}

GameState::~GameState() = default;

void GameState::update(const sf::Time& l_elapsed)
{
    m_elapsed += l_elapsed;

    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    m_gameMap.update(l_elapsed.asSeconds());
    context->m_systemManager.update(l_elapsed.asSeconds());

    float timestep = 1.0F / 60;

    if (m_elapsed.asSeconds() >= timestep)
    {
        m_elapsed -= sf::seconds(timestep);
    }
}

void GameState::draw()
{
    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    auto& system_manager = context->m_systemManager;
    auto& window         = context->m_window;

    m_gameMap.draw();
    system_manager.draw(window);
}

void GameState::activate() {}

void GameState::deactivate() {}

void GameState::onCreate() {}