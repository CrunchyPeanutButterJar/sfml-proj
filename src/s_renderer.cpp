#include <SFML/Graphics/Rect.hpp>
#include <c_position.hpp>
#include <c_drawable.hpp>
#include <c_spritesheet.hpp>
#include <s_renderer.hpp>
#include <systemmanager.hpp>
#include <entitymanager.hpp>

S_Renderer::S_Renderer(SystemManager& l_systemManager) : S_Base{System::Renderer, l_systemManager}
{
    Bitmask req;
    req.turnOnBit((Bitmask::Position)Component::Position);
    req.turnOnBit((Bitmask::Position)Component::SpriteSheet);
    m_requiredComponents.push_back(std::move(req));

    m_systemManager.getMessageHandler().subscribe(EntityMessage::Direction_Changed,this);
}

void S_Renderer::update(float)
{
    auto& entityManager = m_systemManager.getEntityManager();
    for(auto entity : m_entities)
    {
        auto* position = entityManager.getComponent<C_Position>(entity, Component::Position);
        auto* drawable = entityManager.getComponent<C_Drawable>(entity, Component::SpriteSheet);
        drawable->updatePosition(position->getPosition());
    }
}

void S_Renderer::handleEvent(EntityId , EntityEvent )
{
}

void S_Renderer::notify(const Message& l_message)
{
    if(hasEntity(l_message.m_receiver))
    {
        EntityMessage m = (EntityMessage) l_message.m_type;
        switch(m)
        {
            case EntityMessage::Direction_Changed:
            setSheetDirection(l_message.m_receiver,(Direction)l_message.m_int);
            break;
            default:
        }
    }
}

void S_Renderer::render(Window& l_window)
{
    auto& entityManager = m_systemManager.getEntityManager();
    for(auto entity : m_entities)
    {
        auto* position = entityManager.getComponent<C_Position>(entity, Component::Position);
        auto* drawable = entityManager.getComponent<C_Drawable>(entity, Component::SpriteSheet);
        sf::FloatRect drawableBounds;
        drawableBounds.left = position->getPosition().x - (drawable->getSize().x/2.);
        drawableBounds.top = position->getPosition().y - (drawable->getSize().y);
        drawableBounds.height = drawable->getSize().y;
        drawableBounds.width = drawable->getSize().x;
        if(l_window.GetViewSpace().intersects(drawableBounds))
        {
            drawable->draw(l_window.GetRenderWindow());
        }
    }
}

void S_Renderer::setSheetDirection(EntityId l_entity, Direction l_dir)
{
    auto& entityManager = m_systemManager.getEntityManager();
    ASSERT(hasEntity(l_entity), "");
    auto* spriteSheet = entityManager.getComponent<C_SpriteSheet>(l_entity, Component::SpriteSheet);
    spriteSheet->getSpriteSheet()->setDirection(l_dir);
}