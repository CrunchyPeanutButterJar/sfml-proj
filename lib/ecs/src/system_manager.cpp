#include <ecs/system/s_collision.hpp>
#include <ecs/system/s_control.hpp>
#include <ecs/system/s_movement.hpp>
#include <ecs/system/s_renderer.hpp>
#include <ecs/system/s_sheet_animation.hpp>
#include <ecs/system/s_sound.hpp>
#include <ecs/system/s_state.hpp>
#include <ecs/system/system_manager.hpp>
#include <memory>
#include <utils/assert.hpp>

using namespace ecs::system;
using namespace ecs::messaging;
using namespace ecs::entity;

SystemManager::SystemManager(EntityManager& l_entityManager) : m_entityManager{l_entityManager}
{
    constexpr float PhysicsFrameTime = 1 / 60.F;

    m_systems[System::Renderer]       = std::make_unique<SRenderer>(*this);
    m_systems[System::Control]        = std::make_unique<SControl>(*this);
    m_systems[System::Collision]      = std::make_unique<SCollision>(*this);
    m_systems[System::Movement]       = std::make_unique<SMovement>(*this);
    m_systems[System::SheetAnimation] = std::make_unique<SSheetAnimation>(*this);
    m_systems[System::State]          = std::make_unique<SState>(*this);
    m_systems[System::Sound]          = std::make_unique<SSound>(*this);

    m_systems[System::Control]->m_frameTime  = PhysicsFrameTime;
    m_systems[System::Movement]->m_frameTime = PhysicsFrameTime;
    m_systems[System::Collision]->m_frameTime = PhysicsFrameTime;
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
    for (auto& s_itr : m_systems)
    {
        auto* system = s_itr.second.get();
        if (system->fitsRequirements(l_bits))
        {
            if (!system->hasEntity(l_entity))
            {
                system->addEntity(l_entity);
            }
        }
        else
        {
            if (system->hasEntity(l_entity))
            {
                system->removeEntity(l_entity);
            }
        }
    }
}

void SystemManager::removeEntity(EntityId l_entity)
{
    for (auto& s_itr : m_systems)
    {
        s_itr.second->removeEntity(l_entity);
    }
}

void SystemManager::update(float l_dt)
{
    float frame_time = l_dt;

    for(auto& s_itr : m_systems)
    {
        auto* system = s_itr.second.get();
        frame_time = std::min(frame_time, system->m_frameTime.value_or(l_dt));
    }

    static constexpr auto UpdateIteration = [](SystemManager& l_systemManager, float l_frameTime)
    {
        for (auto& s_itr : l_systemManager.m_systems)
        {
            auto* system = s_itr.second.get();
            system->m_elapsed += l_frameTime;
            if (system->m_elapsed >= system->m_frameTime.value_or(l_frameTime))
            {
                system->update(system->m_elapsed);
                system->m_elapsed = 0.F;
            }
        }
        l_systemManager.handleEvents();
    };

    float remaining_time = l_dt;

    for (;remaining_time >= frame_time; remaining_time -= frame_time)
    {
        UpdateIteration(*this, frame_time);
    }

    if(remaining_time > 0.F)
    {
        UpdateIteration(*this, remaining_time);
    }

}

void SystemManager::handleEvents()
{
    for (auto& entity_and_events : m_events)
    {
        auto& [entity, events] = entity_and_events;
        while (auto event = events.processEvent())
        {
            for (auto& s_itr : m_systems)
            {
                if (s_itr.second->hasEntity(entity) && s_itr.second->m_elapsed == 0.F)
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