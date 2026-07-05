#ifndef ECS_SYSTEM_S_BASE_HPP
#define ECS_SYSTEM_S_BASE_HPP

#include <ecs/ecs_types.hpp>
#include <ecs/messaging/entity_events.hpp>
#include <ecs/messaging/observer.hpp>
#include <ecs/system/system_manager.fwd.hpp>
#include <memory>
#include <utils/bitmask.hpp>
#include <vector>

namespace ecs::system
{
using EntityList   = std::vector<EntityId>;
using Requirements = std::vector<utils::Bitmask>;

class SBase : public messaging::Observer
{
  public:
    SBase(System l_id, SystemManager& l_sysManager);
    ~SBase() override = default;

    auto addEntity(EntityId l_entity) -> bool;
    auto hasEntity(EntityId l_entity) -> bool;
    auto removeEntity(EntityId l_entity) -> bool;

    [[nodiscard]] auto getId() const -> System;

    auto fitsRequirements(utils::Bitmask l_bits) -> bool;

    virtual void update(float l_dt)                                             = 0;
    virtual void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) = 0;

  protected:
    SystemManager& m_systemManager;
    System         m_id;
    Requirements   m_requiredComponents;
    EntityList     m_entities;
};

using SBasePtr = std::unique_ptr<SBase>;
} // namespace ecs::system

#endif