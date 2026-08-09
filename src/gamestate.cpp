#include "core/directions.hpp"
#include "core/gui/GUI_interface.hpp"
#include "ecs/ecs_types.hpp"
#include "ecs/messaging/message_handler.hpp"
#include "ecs/shared_context.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <core/audio/sound_manager.hpp>
#include <core/gui/GUI_manager.hpp>
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
                                             core::GuiEventClick{"DebugOverlay",
                                                                 "ToggleSpriteSheetOverlay"}),
                                     core::NonCustomizable>;
template class core::RegisterBinding<BINDING("Game_ToggleCollidableDebugOverlay",
                                             core::GuiEventClick{"DebugOverlay",
                                                                 "ToggleCollidableDebugOverlay"}),
                                     core::NonCustomizable>;

template class core::RegisterBinding<
    BINDING("Game_GoToMainMenu", core::KeyPressed{sf::Keyboard::Escape}), core::NonCustomizable>;

template class core::RegisterBinding<
    BINDING("Game_OpenConsole", core::KeyPressed{sf::Keyboard::Tilde}), core::Customizable>;

template class core::RegisterBinding<BINDING("Game_ToggleDebugOverlay",
                                             core::KeyReleased{sf::Keyboard::O},
                                             core::KeyPressed{sf::Keyboard::LShift}),
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

template class core::RegisterBinding<BINDING("Game_OpenGameMap", core::KeyPressed{sf::Keyboard::M}),
                                     core::Customizable>;

GameState::GameState(core::state::StateManager& l_stateManager)
    : BaseState(l_stateManager), m_gameMap{m_stateManager.getContext<ecs::SharedContext>(), *this}
{
    m_gameMap.loadMap(utils::getConfigDirectory() + "map.map");

    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    auto& gui_manager = m_stateManager.getContext()->m_guiManager;
    gui_manager.loadInterface(StateType::Game, "DebugOverlay.interface", "DebugOverlay");
    auto* interface = gui_manager.getInterface(StateType::Game, "DebugOverlay");
    interface->setPosition({0, 16});
    interface->setActive(false);

    gui_manager.loadInterface(StateType::Game, "Notepad.interface", "Console");
    auto* console = gui_manager.getInterface(StateType::Game, "Console");
    console->setPosition({0, 16});
    console->setActive(false);

    gui_manager.loadInterface(StateType::Game, "Map.interface", "Map");
    auto* map = gui_manager.getInterface(StateType::Game, "Map");
    map->setPosition({0, 16});
    map->setActive(false);

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

    event_manager.addCallback(
        StateType::Game, "Game_ToggleSpriteSheetOverlay", [](const auto&)
        { ecs::entity::CSpriteSheet::debug_overlay = !ecs::entity::CSpriteSheet::debug_overlay; });

    event_manager.addCallback(
        StateType::Game, "Game_ToggleCollidableDebugOverlay", [](const auto&)
        { ecs::entity::CCollidable::debug_overlay = !ecs::entity::CCollidable::debug_overlay; });

    event_manager.addCallback(StateType::Game, "Game_ToggleDebugOverlay", [interface](const auto&)
                              { interface->setActive(!interface->isActive()); });

    event_manager.addCallback(StateType::Game, "Game_OpenConsole",
                              [console](const auto& l_details)
                              {
                                  if (!l_details.m_realtimeContribution)
                                  {
                                      console->setActive(!console->isActive());
                                  }
                              });

    event_manager.addCallback(StateType::Game, "Game_OpenGameMap",
                              [map](const core::EventDetails& l_details)
                              {
                                  if (!l_details.m_realtimeContribution)
                                  {
                                      map->setActive(!map->isActive());
                                  }
                              });

    auto* state_manager = &m_stateManager;

    event_manager.addCallback(StateType::Game, "Game_GoToMainMenu",
                              [state_manager](const core::EventDetails& l_details)
                              {
                                  if (!l_details.m_realtimeContribution)
                                  {
                                      state_manager->switchTo(StateType::MainMenu);
                                  }
                              });

    auto& sound_manager = m_stateManager.getContext()->m_soundManager;
    sound_manager.playMusic("media/audio/game_theme.mp3", 50.F, true);
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