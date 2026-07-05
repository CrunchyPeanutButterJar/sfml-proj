#ifndef ECS_SYSTEM_SYSTEM_MANAGER_HPP
#define ECS_SYSTEM_SYSTEM_MANAGER_HPP

#include <core/window.fwd.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/entity/entity_manager.fwd.hpp>
#include <ecs/messaging/event_queue.hpp>
#include <ecs/messaging/message_handler.hpp>
#include <ecs/system/s_base.hpp>
#include <ecs/system/system_manager.fwd.hpp>
#include <unordered_map>
#include <utils/bitmask.hpp>

namespace ecs::system
{

using SystemContainer      = std::unordered_map<System, SBasePtr>;
using EntityEventContainer = std::unordered_map<EntityId, messaging::EventQueue>;

class SystemManager
{
  public:
    SystemManager(entity::EntityManager& l_entityManager);

    auto getEntityManager() -> entity::EntityManager&;
    auto getMessageHandler() -> messaging::MessageHandler&;

    template <typename T> auto getSystem(System l_system) -> T*
    {
        auto itr = m_systems.find(l_system);
        return itr != m_systems.end() ? dynamic_cast<T*>(itr->second.get()) : nullptr;
    }

    void addEvent(EntityId l_entity, messaging::EventId l_event);

    void entityModified(EntityId l_id, utils::Bitmask l_bits);
    void removeEntity(EntityId l_id);

    void update(float l_dt);
    void handleEvents();
    void draw(core::Window& l_window);

  private:
    entity::EntityManager&    m_entityManager;
    SystemContainer           m_systems;
    EntityEventContainer      m_events;
    messaging::MessageHandler m_messageHandler;
};
} // namespace ecs::system

#endif