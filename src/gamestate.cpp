#include "core/directions.hpp"
#include <SFML/Graphics/View.hpp>
#include <core/audio/sound_manager.hpp>
#include <core/bindings.hpp>
#include <core/event_manager.hpp>
#include <core/state/basestate.hpp>
#include <core/state/statemanager.hpp>
#include <core/window.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/entity/c_collidable.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/entity_manager.hpp>
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

template class core::RegisterBinding<BINDING("Game_MoveRight", core::KeyPressed{sf::Keyboard::D}),
                                     core::Customizable>;
template class core::RegisterBinding<BINDING("Game_MoveLeft", core::KeyPressed{sf::Keyboard::A}),
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

auto GameState::playerHasLost() -> bool
{
    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    auto&       entity_manager = context->m_entityManager;
    const auto& player_aabb =
        entity_manager
            .getComponent<ecs::entity::CCollidable>(m_map.getPlayerId(), ecs::Component::Collidable)
            ->getCollidable();
    return !core::getViewSpace(m_view).intersects(player_aabb);
}

void GameState::update(const sf::Time& l_elapsed)
{
    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    updateCamera(l_elapsed);
    if (auto offset_y = m_map.requiresTilesMapRegeneration(); offset_y > 0.)
    {
        auto new_center = m_view.getCenter();
        new_center.y += offset_y;
        m_view.setCenter(new_center);

        m_map.transitionToNextGif();
    }
    m_map.update(l_elapsed.asSeconds());
    context->m_systemManager.update(l_elapsed.asSeconds());

    if (playerHasLost())
    {
        auto& sound_manager = context->m_soundManager;

        context->m_entityManager.removeEntity(m_map.getPlayerId());
        m_stateManager.remove(GameState::TYPE);
        m_stateManager.switchTo(StateType::MainMenu);
        ASSERT_NON_FATAL(sound_manager.play("lose", {0, 0, 0}, false, true).has_value(),
                         "Failed to play lose sound");
    }
}

void GameState::draw()
{
    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    m_map.draw();
    context->m_systemManager.draw(context->m_window);
}

void GameState::updateCamera(const sf::Time& /*l_elapsed*/)
{
    const auto OldViewCenter = m_view.getCenter();

    auto* context    = m_stateManager.getContext<ecs::SharedContext>();
    auto* player_pos = context->m_entityManager.getComponent<ecs::entity::CPosition>(
        m_map.getPlayerId(), ecs::Component::Position);
    const auto PlayerPos = player_pos->getPosition();

    if (PlayerPos.y < OldViewCenter.y)
    {
        m_view.setCenter(OldViewCenter.x, PlayerPos.y);
    }
}
