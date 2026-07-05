#include <ecs/system/systemmanager.hpp>
#include <ecs/system/s_renderer.hpp>
#include <utils/assert.hpp>

using namespace ecs::system;
using namespace ecs::messaging;
using namespace ecs::entity;

SystemManager::SystemManager(EntityManager& l_entityManager) : m_entityManager{l_entityManager}
{
    m_systems[System::Renderer] = std::make_unique<SRenderer>(*this);
}

auto SystemManager::getEntityManager() -> EntityManager&
{
    return m_entityManager;
}

auto SystemManager::getMessageHandler() -> MessageHandler&
{
    return m_messageHandler;
}

void SystemManager::addEvent(EntityId l_entity, EventId l_event)
{
    m_events[l_entity].addEvent(l_event);
}

void SystemManager::entityModified(EntityId l_entity, utils::Bitmask l_bits)
{
    for(auto& s_itr : m_systems)
    {
        auto* system = s_itr.second.get();
        if(system->fitsRequirements(l_bits))
        {
            if(!system->hasEntity(l_entity))
            {
                system->addEntity(l_entity);
            }
        }
        else
        {
            if(system->hasEntity(l_entity))
            {
                system->removeEntity(l_entity);
            }
        }
    }
}

void SystemManager::removeEntity(EntityId l_entity)
{
    for(auto& s_itr : m_systems)
    {
        s_itr.second->removeEntity(l_entity);
    }
}

void SystemManager::update(float l_dt)
{
    for(auto& s_itr : m_systems)
    {
        s_itr.second->update(l_dt);
    }
    handleEvents();
}

void SystemManager::handleEvents()
{
    for(auto& entity_and_events : m_events)
    {
        auto& [entity, events] = entity_and_events;
        while(auto event = events.processEvent())
        {
            for(auto& s_itr : m_systems)
            {
                if(s_itr.second->hasEntity(entity))
                {
                    s_itr.second->handleEvent(entity, (EntityEvent)*event);
                }
            }
        }
    }
}

void SystemManager::draw(core::Window& l_window)
{
    auto* s_renderer = getSystem<SRenderer>(System::Renderer);
    ASSERT(s_renderer, "Renderer not initialized!!");
    s_renderer->render(l_window);
}