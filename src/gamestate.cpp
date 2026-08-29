#include "core/directions.hpp"
#include "ecs/entity/c_state.fwd.hpp"
#include "ecs/messaging/entity_message.hpp"
#include "ecs/messaging/message.hpp"
#include "utils/assert.hpp"
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <core/audio/sound_manager.hpp>
#include <core/bindings.hpp>
#include <core/event_manager.hpp>
#include <core/gui/GUI_manager.hpp>
#include <core/state/basestate.hpp>
#include <core/state/statemanager.hpp>
#include <core/window.hpp>
#include <cstdint>
#include <ecs/ecs_types.hpp>
#include <ecs/entity/c_collidable.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/c_spritesheet.hpp>
#include <ecs/entity/c_state.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/messaging/message_handler.hpp>
#include <ecs/shared_context.hpp>
#include <ecs/system/s_collision.hpp>
#include <ecs/system/s_control.hpp>
#include <ecs/system/s_movement.hpp>
#include <ecs/system/system_manager.hpp>
#include <gamestate.hpp>
#include <utils/utilities.hpp>

enum EntityTag : std::uint8_t
{
    Player = 0,
    Demon
};

static bool s_init = []() -> bool
{
    ecs::system::registerCollisionResolution(
        Demon, Player,
        [](ecs::EntityId l_playerId, ecs::EntityId l_demonId, ecs::SharedContext* context)
        {
            auto& entity_manager = context->m_entityManager;
            auto& system_manager = context->m_systemManager;

            auto* state =
                entity_manager.getComponent<ecs::entity::CState>(l_playerId, ecs::Component::State);
            if (state->getState() != ecs::entity::EntityState::Attacking)
            {
                return;
            }

            auto* spritesheet = entity_manager.getComponent<ecs::entity::CSpriteSheet>(
                l_playerId, ecs::Component::SpriteSheet);
            if (spritesheet->getSpriteSheet()->getCurrentAnimation()->isInAction())
            {
                LOG("Killing {}", l_demonId);
                auto& msg_handler = system_manager.getMessageHandler();

                ecs::messaging::Message msg{
                    .m_type =
                        (ecs::messaging::MessageType)ecs::messaging::EntityMessage::Switch_State,
                    .m_receiver = (int)l_demonId,
                    .m_int      = (int)ecs::entity::EntityState::Dying};

                msg_handler.dispatch(msg);
            }
        });
    return true;
}();

REGISTER_BINDING(core::Customizable, "Game_MoveRight", core::KeyPressed{sf::Keyboard::Right});
REGISTER_BINDING(core::Customizable, "Game_MoveLeft", core::KeyPressed{sf::Keyboard::Left});
REGISTER_BINDING(core::Customizable, "Game_MoveRight", core::KeyPressed{sf::Keyboard::D});
REGISTER_BINDING(core::Customizable, "Game_MoveLeft", core::KeyPressed{sf::Keyboard::A});
REGISTER_BINDING(core::Customizable, "Game_Jump", core::KeyPressed{sf::Keyboard::Space});
REGISTER_BINDING(core::Customizable, "Game_Attack", core::KeyPressed{sf::Keyboard::E});

static void jumpEntity(ecs::messaging::MessageHandler& l_messageHandler, ecs::EntityId l_entity)
{
    ecs::messaging::Message msg{
        .m_type     = (ecs::messaging::MessageType)ecs::messaging::EntityMessage::Jump,
        .m_receiver = (int)l_entity,
        .m_bool     = false /*ignored*/};
    l_messageHandler.dispatch(msg);
}

constexpr const char* SCORE_INTERFACE_NAME = "ScoreInterface";

GameState::GameState(core::state::StateManager& l_stateManager)
    : core::state::BaseState{l_stateManager},
      m_map{m_stateManager.getContext<ecs::SharedContext>(), *this},
      m_enemyEntitiesManager{m_stateManager.getContext<ecs::SharedContext>()->m_entityManager,
                             m_map}
{
    m_map.loadMap(utils::getConfigDirectory() + "map.map");

    m_enemyEntitiesManager.generateEnemies();

    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    auto& gui_manager = context->m_guiManager;
    gui_manager.loadInterface(StateType::Game, "Score.interface", SCORE_INTERFACE_NAME);

    auto* system_manager = &context->m_systemManager;

    auto* s_collision = system_manager->getSystem<ecs::system::SCollision>(ecs::System::Collision);
    auto* s_movement  = system_manager->getSystem<ecs::system::SMovement>(ecs::System::Movement);
    s_collision->setMap(&m_map);
    s_movement->setMap(&m_map);

    auto& sound_manager = context->m_soundManager;

    sound_manager.playMusic("media/audio/game_theme.wav", 70.F, true);

    ecs::EntityId player_id = m_map.getPlayerId();

    auto& event_manager = context->m_eventManager;
    event_manager.addCallback(StateType::Game, "Game_MoveRight",
                              [player_id, system_manager](const auto&)
                              {
                                  ecs::system::moveEntity(system_manager->getMessageHandler(),
                                                          player_id, core::Direction::Right);
                              });

    event_manager.addCallback(StateType::Game, "Game_MoveLeft",
                              [player_id, system_manager](const auto&)
                              {
                                  ecs::system::moveEntity(system_manager->getMessageHandler(),
                                                          player_id, core::Direction::Left);
                              });

    event_manager.addCallback(
        StateType::Game, "Game_Attack", [player_id, system_manager](const auto&)
        { ecs::system::entityAttack(system_manager->getMessageHandler(), player_id); });

    auto& player_has_jumped = m_playerHasJumped;
    event_manager.addCallback(
        StateType::Game, "Game_Jump", [player_id, system_manager, &player_has_jumped](const auto&)
        { jumpEntity(system_manager->getMessageHandler(), player_id), player_has_jumped = true; });
}

GameState::~GameState()
{
    auto* context       = m_stateManager.getContext<ecs::SharedContext>();
    auto& event_manager = context->m_eventManager;

    event_manager.removeCallback(StateType::Game, "Game_MoveLeft");
    event_manager.removeCallback(StateType::Game, "Game_MoveRight");
    event_manager.removeCallback(StateType::Game, "Game_Jump");
    event_manager.removeCallback(StateType::Game, "Game_Attack");

    context->m_guiManager.removeInterface(GameState::TYPE, SCORE_INTERFACE_NAME);
}

static auto cameraAteEntity(ecs::EntityId l_entity, const sf::View& l_view,
                            ecs::entity::EntityManager& l_entityManager) -> bool
{
    const auto& entity_aabb =
        l_entityManager
            .getComponent<ecs::entity::CCollidable>(l_entity, ecs::Component::Collidable)
            ->getCollidable();

    const auto& view_space = core::getViewSpace(l_view);

    return entity_aabb.top > (view_space.top + view_space.height);
}

auto GameState::playerHasLost() -> bool
{
    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    return cameraAteEntity(m_map.getPlayerId(), m_view, context->m_entityManager);
}

void GameState::updateScore()
{
    if (!m_playerHasJumped)
    {
        return;
    }

    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    auto& entity_manager = context->m_entityManager;
    auto* player_pos     = entity_manager.getComponent<ecs::entity::CPosition>(
        m_map.getPlayerId(), ecs::Component::Position);
    double potential_new_score =
        -player_pos->getPosition().y + (float)context->m_window.getWindowSize().y;

    auto&       gui_manager     = context->m_guiManager;
    auto*       score_interface = gui_manager.getInterface(GameState::TYPE, SCORE_INTERFACE_NAME);
    auto*       score_element   = score_interface->getElement("Score");
    auto        score_text      = score_element->getText();
    std::string score_label;
    double      new_score = 0.F;
    std::stringstream ss(score_text);
    ss >> score_label >> new_score;

    new_score = std::max(potential_new_score + m_offsetY, new_score);
    score_element->setText(score_label + " " + std::to_string((unsigned int)new_score));
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
        m_offsetY += offset_y;

        m_enemyEntitiesManager.generateEnemies(-1);
    }
    m_map.update(l_elapsed.asSeconds());
    context->m_systemManager.update(l_elapsed.asSeconds());
    updateScore();

    auto* this_state = this;

    m_enemyEntitiesManager.removeEntities(
        [this_state](ecs::EntityId l_entity) -> bool
        {
            return cameraAteEntity(
                l_entity, this_state->m_view,
                this_state->m_stateManager.getContext<ecs::SharedContext>()->m_entityManager);
        });

    if (playerHasLost())
    {
        auto& sound_manager = context->m_soundManager;

        context->m_entityManager.removeEntity(m_map.getPlayerId());
        m_stateManager.remove(GameState::TYPE);
        m_stateManager.switchTo(StateType::MainMenu);
        sound_manager.play("lose", {0, 0, 0}, false, true);
    }
}

void GameState::draw()
{
    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    m_map.draw();
    context->m_systemManager.draw(context->m_window);
}

auto GameState::getScore() -> unsigned int
{
    auto* context = m_stateManager.getContext<ecs::SharedContext>();

    auto&       gui_manager     = context->m_guiManager;
    auto*       score_interface = gui_manager.getInterface(GameState::TYPE, SCORE_INTERFACE_NAME);
    auto*       score_element   = score_interface->getElement("Score");
    auto        score_text      = score_element->getText();
    std::string score_label;
    double      score = 0.F;
    std::stringstream ss(score_text);
    ss >> score_label >> score;

    return (unsigned int)score;
}

void GameState::updateCamera(const sf::Time& l_elapsed)
{
    constexpr double MaxSpeed = 320.; /*half screen size*/

    if (!m_playerHasJumped)
    {
        return;
    }

    const float DifficultyFactor = std::min((std::sqrt(getScore())) + 10., MaxSpeed);
    const float CameraSpeed      = -1.F * DifficultyFactor;

    const auto  OldViewCenter  = m_view.getCenter();
    const float NewViewCenterY = OldViewCenter.y + (CameraSpeed * l_elapsed.asSeconds());

    auto* context    = m_stateManager.getContext<ecs::SharedContext>();
    auto* player_pos = context->m_entityManager.getComponent<ecs::entity::CPosition>(
        m_map.getPlayerId(), ecs::Component::Position);
    const auto PlayerPos = player_pos->getPosition();

    m_view.setCenter(OldViewCenter.x, std::min(PlayerPos.y, NewViewCenterY));
}
