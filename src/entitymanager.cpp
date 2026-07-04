#include <utils/assert.hpp>
#include <utils/utilities.hpp>
#include <c_position.hpp>
#include <ecs_types.hpp>
#include <entitymanager.hpp>
#include <textureManager.hpp>
#include <systemmanager.hpp>
#include <c_spritesheet.hpp>

EntityManager::EntityManager(SystemManager& l_sysManager, TextureManager& l_textureManager):
m_systemManager(l_sysManager),
m_textureManager(l_textureManager),
m_idCounter{0}
{
    addComponentType<C_Position>(Component::Position);
    addComponentType<C_SpriteSheet>(Component::SpriteSheet);
}

int EntityManager::addEntity(Bitmask l_mask)
{
    EntityId entity = m_idCounter++;
    ASSERT(m_entities.emplace(entity, EntityData{}).second, "Invalid Entity Id Counter {}", entity);
    for(size_t i = 0; i < N_COMPONENT_TYPES;i++)
    {
        if(l_mask.getBit(i))
        {
            addComponent(entity, (Component)i);
        }
    }
    m_systemManager.entityModified(entity, l_mask);
    m_systemManager.addEvent(entity, (EventId)EntityEvent::Spawned);
    return entity;
}

int EntityManager::addEntity(const std::string& l_entityFile)
{
    static const std::string EntityDir = Utils::GetResourcesDirectory() + "media/entities/";
    int entityId = -1;
    auto fileStream = Utils::ReadFile(EntityDir + l_entityFile);
    if(!fileStream)
    {
        FAILURE_NON_FATAL("Could not open entity file {}", l_entityFile);
        return entityId;
    }
    Utils::Tokens tokens{std::move(*fileStream)};
    
    while(!tokens.empty())
    {
        auto key = *ConsumeToken<std::string>(tokens);
        if(key == "Name")
        {

        }
        else if(key == "Attributes")
        {
            if(entityId != -1) 
            {
                FAILURE_NON_FATAL("Invalid entity file {} : duplicate `Attributes` field found", l_entityFile);
                return -1;
            }
            entityId = addEntity(*ConsumeToken<Bitset>(tokens));
        }
        else if(key == "Component")
        {
            if(entityId == -1)
            {
                FAILURE_NON_FATAL("Invalid entity file {} : `Component` field found before `Attributes` field ", l_entityFile);
                return -1;
            }
            const auto c_id = *ConsumeToken<unsigned int>(tokens);
            auto* component = getComponent<C_Base>(entityId, (Component) c_id);
            if(!component)
            {
                FAILURE_NON_FATAL("Could not find component {}", c_id);
                return -1;
            }
            component->readInput(tokens);
            if(auto* c_spriteSheet = static_cast<C_SpriteSheet*>(component); component->getType() == Component::SpriteSheet)
            {
                c_spriteSheet->create(m_textureManager);
            }
        }
    }

    return entityId;
}

bool EntityManager::removeEntity(EntityId l_id)
{
    auto itr = m_entities.find(l_id);
    if(itr == m_entities.end())
    {
        return false;
    }
    m_entities.erase(itr);
    m_systemManager.removeEntity(l_id);
    return true;
}

bool EntityManager::addComponent(EntityId l_entity, Component l_component)
{
    auto itr = m_entities.find(l_entity);
    if(itr == m_entities.end() || itr->second.first.getBit((unsigned int)l_component))
    {
        return false;
    }
    auto itr2 = m_componentFactory.find(l_component);
    if(itr2 == m_componentFactory.end())
    {
        FAILURE_NON_FATAL("Could not find Component Factory for {}", (const unsigned int&) l_component);
        return false;
    }
    auto newComponent = itr2->second();
    itr->second.second.emplace_back(std::move(newComponent));
    itr->second.first.turnOnBit((unsigned int)l_component);
    m_systemManager.entityModified(l_entity, itr->second.first);
    return true;
}

bool EntityManager::removeComponent(EntityId l_entity, Component l_component)
{
    auto itr = m_entities.find(l_entity);
    if(itr == m_entities.end() || !itr->second.first.getBit((unsigned int)l_component))
    {
        return false;
    }
    auto& container = itr->second.second;
    auto component = std::find_if(container.begin(), container.end(), [l_component](const auto& component){return component->getType() == l_component;});
    if(component == container.end())
    {
        FAILURE_NON_FATAL("Could not find component {}", (const unsigned int&)l_component);
        return false;
    }
    container.erase(component);
    itr->second.first.clearBit((unsigned int)l_component);
    m_systemManager.entityModified(l_entity, itr->second.first);
    return true;
}

bool EntityManager::hasComponent(EntityId l_entity, Component l_component)
{
    auto itr = m_entities.find(l_entity);
    if(itr == m_entities.end())
    {
        return false;
    }
    return itr->second.first.getBit((unsigned int)l_component);
}