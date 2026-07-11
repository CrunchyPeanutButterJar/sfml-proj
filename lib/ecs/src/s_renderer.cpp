#include <SFML/Graphics/Rect.hpp>
#include <core/window.hpp>
#include <ecs/entity/c_drawable.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/c_spritesheet.hpp>
#include <ecs/entity/entity_manager.hpp>
#include <ecs/system/s_renderer.hpp>
#include <ecs/system/system_manager.hpp>

using namespace ecs::system;
using namespace ecs::messaging;
using namespace ecs::entity;

SRenderer::SRenderer(SystemManager& l_systemManager) : SBase{System::Renderer, l_systemManager}
{
    utils::Bitmask req;
    req.turnOnBit((utils::Bitmask::Position)Component::Position);
    req.turnOnBit((utils::Bitmask::Position)Component::SpriteSheet);
    m_requiredComponents.push_back(req);

    m_systemManager.getMessageHandler().subscribe(EntityMessage::Direction_Changed, this);
}

void SRenderer::update(float /*l_dt*/)
{
    auto& entity_manager = m_systemManager.getEntityManager();
    for (auto entity : m_entities)
    {
        auto* position = entity_manager.getComponent<CPosition>(entity, Component::Position);
        auto* drawable = entity_manager.getComponent<CDrawable>(entity, Component::SpriteSheet);
        drawable->updatePosition(position->getPosition());
    }
}

void SRenderer::handleEvent(EntityId /*l_entity*/, EntityEvent /*l_event*/) {}

void SRenderer::notify(const Message& l_message)
{
    if (hasEntity(l_message.m_receiver))
    {
        auto m = (EntityMessage)l_message.m_type;
        switch (m)
        {
        case EntityMessage::Direction_Changed:
            setSheetDirection(l_message.m_receiver, (core::Direction)l_message.m_int);
            break;
        default:
        }
    }
}

static void doDraw(core::Window& l_window, ecs::entity::CPosition* l_position,
                   ecs::entity::CDrawable* l_drawable)
{
    sf::FloatRect drawable_bounds;
    drawable_bounds.left   = l_position->getPosition().x - (l_drawable->getSize().x / 2.);
    drawable_bounds.top    = l_position->getPosition().y - (l_drawable->getSize().y);
    drawable_bounds.height = l_drawable->getSize().y;
    drawable_bounds.width  = l_drawable->getSize().x;
    if (l_window.getViewSpace().intersects(drawable_bounds))
    {
        l_drawable->draw(l_window.getRenderWindow());
    }
}

void SRenderer::render(core::Window& l_window)
{
    auto& entity_manager = m_systemManager.getEntityManager();
    for (auto entity : m_entities)
    {
        auto* position = entity_manager.getComponent<CPosition>(entity, Component::Position);
        auto* drawable = entity_manager.getComponent<CDrawable>(entity, Component::SpriteSheet);
        doDraw(l_window, position, drawable);
        if (auto* collidable =
                entity_manager.getComponent<CDrawable>(entity, Component::Collidable))
        {
            doDraw(l_window, position, collidable);
        }
    }
}

void SRenderer::setSheetDirection(EntityId l_entity, core::Direction l_dir)
{
    auto& entity_manager = m_systemManager.getEntityManager();
    ASSERT(hasEntity(l_entity), "");
    auto* sprite_sheet =
        entity_manager.getComponent<CSpriteSheet>(l_entity, Component::SpriteSheet);
    sprite_sheet->getSpriteSheet()->setDirection(l_dir);
}