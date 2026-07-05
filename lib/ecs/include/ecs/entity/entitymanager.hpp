#ifndef ECS_ENTITY_ENTITYMANAGER_HPP
#define ECS_ENTITY_ENTITYMANAGER_HPP

#include <core/graphics/textureManager.fwd.hpp>
#include <ecs/system/systemmanager.fwd.hpp>
#include <ecs/entity/c_base.hpp>
#include <vector>
#include <functional>
#include <utils/bitmask.hpp>
#include <utility>
#include <unordered_map>

namespace ecs::entity
{
using ComponentContainer = std::vector<CBasePtr>;
using EntityData = std::pair<utils::Bitmask, ComponentContainer>;
using EntityContainer = std::unordered_map<EntityId, EntityData>;
using ComponentFactory = std::unordered_map<Component, std::function<CBasePtr()>>;

class EntityManager
{
public:
    EntityManager(system::SystemManager& l_sysManager, core::graphics::TextureManager& l_textureManager);

    int addEntity(utils::Bitmask l_mask);
    int addEntity(const std::string& l_entityFile);
    bool removeEntity(EntityId l_id);

    bool addComponent(EntityId l_entity, Component l_component);

    bool removeComponent(EntityId l_entity,Component l_component);
    bool hasComponent(EntityId l_entity, Component l_component);

    template<class T>
    T* getComponent(EntityId l_entity, Component l_component)
    {
        auto itr = m_entities.find(l_entity);
        if(itr == m_entities.end() || !itr->second.first.getBit((unsigned int)l_component))
        {
            return nullptr;
        }
        auto& components = itr->second.second;
        auto component = std::find_if(components.begin(), components.end(),
        [l_component](const CBasePtr& l_c)
        {
            return l_c->getType() == l_component;
        });

        ASSERT_NON_FATAL(component != components.end(), "Could not find component!");
        return component == components.end() ? nullptr : dynamic_cast<T*>(component->get());
    }

private:
    template<class T>
    void addComponentType(Component l_id)
    {
        m_componentFactory[l_id] = []() -> CBasePtr {return std::make_unique<T>();};
    }


    system::SystemManager& m_systemManager;
    core::graphics::TextureManager& m_textureManager;

    EntityId m_idCounter{0};
    EntityContainer m_entities;
    ComponentFactory m_componentFactory;
};
};

#endif