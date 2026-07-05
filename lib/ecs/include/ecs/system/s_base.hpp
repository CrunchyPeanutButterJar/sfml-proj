#ifndef ECS_SYSTEM_S_BASE_HPP
#define ECS_SYSTEM_S_BASE_HPP

#include <ecs/system/systemmanager.fwd.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/messaging/entityevents.hpp>
#include <ecs/messaging/observer.hpp>
#include <utils/bitmask.hpp>
#include <vector>
#include <memory>

namespace ecs::system
{
using EntityList = std::vector<EntityId>;
using Requirements = std::vector<utils::Bitmask>;

class SBase : public messaging::Observer
{
public:
    SBase(System l_id, SystemManager& l_sysManager);
    ~SBase() override = default;

    bool addEntity(EntityId l_entity);
    bool hasEntity(EntityId l_entity);
    bool removeEntity(EntityId l_entity);

    System getId() const;

    bool fitsRequirements(utils::Bitmask l_bits);

    virtual void update(float l_dt) = 0;
    virtual void handleEvent(EntityId l_entity, messaging::EntityEvent l_event) = 0;

protected:
    SystemManager& m_systemManager;
    System m_id;
    Requirements m_requiredComponents;
    EntityList m_entities;
};

using SBasePtr = std::unique_ptr<SBase>;
};

#endif