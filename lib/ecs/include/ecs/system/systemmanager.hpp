#ifndef SYSTEMMANAGER_HPP
#define SYSTEMMANAGER_HPP

#include <ecs/system/s_base.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/messaging/eventqueue.hpp>
#include <utils/bitmask.hpp>
#include <ecs/system/systemmanager.fwd.hpp>
#include <ecs/entity/entitymanager.fwd.hpp>
#include <ecs/messaging/messagehandler.hpp>
#include <core/window.fwd.hpp>
#include <unordered_map>

using SystemContainer = std::unordered_map<System, S_BasePtr>;
using EntityEventContainer = std::unordered_map<EntityId, EventQueue>;

class SystemManager
{
public:
    SystemManager(EntityManager& l_entityManager);

    EntityManager& getEntityManager();
    MessageHandler& getMessageHandler();

    template<typename T>
    T* getSystem(System l_system)
    {
        auto itr = m_systems.find(l_system);
        return itr != m_systems.end() ? dynamic_cast<T*>(itr->second.get()) : nullptr;
    }

    void addEvent(EntityId l_entity, EventId l_event);

    void entityModified(EntityId l_id, Bitmask l_bits);
    void removeEntity(EntityId l_id);

    void update(float l_dt);
    void handleEvents();
    void draw(Window& l_window);
private:
    EntityManager& m_entityManager;
    SystemContainer m_systems;
    EntityEventContainer m_events;
    MessageHandler m_messageHandler;
};

#endif