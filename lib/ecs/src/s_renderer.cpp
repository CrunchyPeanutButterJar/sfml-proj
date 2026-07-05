#include <SFML/Graphics/Rect.hpp>
#include <ecs/entity/entitymanager.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/c_drawable.hpp>
#include <ecs/entity/c_spritesheet.hpp>
#include <ecs/system/s_renderer.hpp>
#include <ecs/system/systemmanager.hpp>

using namespace ecs::system;
using namespace ecs::messaging;
using namespace ecs::entity;

SRenderer::SRenderer(SystemManager& l_systemManager) : SBase{System::Renderer, l_systemManager}
{
    utils::Bitmask req;
    req.turnOnBit((utils::Bitmask::Position)Component::Position);
    req.turnOnBit((utils::Bitmask::Position)Component::SpriteSheet);
    m_requiredComponents.push_back(std::move(req));

    m_systemManager.getMessageHandler().subscribe(EntityMessage::Direction_Changed,this);
}

void SRenderer::update(float /*l_dt*/)
{
    auto& entity_manager = m_systemManager.getEntityManager();
    for(auto entity : m_entities)
    {
        auto* position = entity_manager.getComponent<CPosition>(entity, Component::Position);
        auto* drawable = entity_manager.getComponent<CDrawable>(entity, Component::SpriteSheet);
        drawable->updatePosition(position->getPosition());
    }
}

void SRenderer::handleEvent(EntityId  /*l_entity*/, EntityEvent  /*l_event*/)
{
}

void SRenderer::notify(const Message& l_message)
{
    if(hasEntity(l_message.m_receiver))
    {
        EntityMessage m = (EntityMessage) l_message.m_type;
        switch(m)
        {
            case EntityMessage::Direction_Changed:
            setSheetDirection(l_message.m_receiver,(core::Direction)l_message.m_int);
            break;
            default:
        }
    }
}

void SRenderer::render(core::Window& l_window)
{
    auto& entity_manager = m_systemManager.getEntityManager();
    for(auto entity : m_entities)
    {
        auto* position = entity_manager.getComponent<CPosition>(entity, Component::Position);
        auto* drawable = entity_manager.getComponent<CDrawable>(entity, Component::SpriteSheet);
        sf::FloatRect drawable_bounds;
        drawable_bounds.left = position->getPosition().x - (drawable->getSize().x/2.);
        drawable_bounds.top = position->getPosition().y - (drawable->getSize().y);
        drawable_bounds.height = drawable->getSize().y;
        drawable_bounds.width = drawable->getSize().x;
        if(l_window.getViewSpace().intersects(drawable_bounds))
        {
            drawable->draw(l_window.getRenderWindow());
        }
    }
}

void SRenderer::setSheetDirection(EntityId l_entity, core::Direction l_dir)
{
    auto& entity_manager = m_systemManager.getEntityManager();
    ASSERT(hasEntity(l_entity), "");
    auto* sprite_sheet = entity_manager.getComponent<CSpriteSheet>(l_entity, Component::SpriteSheet);
    sprite_sheet->getSpriteSheet()->setDirection(l_dir);
}