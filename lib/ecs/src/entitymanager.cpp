#include <utils/assert.hpp>
#include <utils/utilities.hpp>
#include <ecs/entity/c_position.hpp>
#include <ecs/entity/c_spritesheet.hpp>
#include <ecs/ecs_types.hpp>
#include <ecs/entity/entitymanager.hpp>
#include <core/graphics/textureManager.hpp>
#include <ecs/system/systemmanager.hpp>

EntityManager::EntityManager(SystemManager& l_sysManager, TextureManager& l_textureManager):
m_systemManager(l_sysManager),
m_textureManager(l_textureManager)

{
    addComponentType<CPosition>(Component::Position);
    addComponentType<CSpriteSheet>(Component::SpriteSheet);
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
    static const std::string EntityDir = Utils::getResourcesDirectory() + "media/entities/";
    int entity_id = -1;
    auto file_stream = Utils::readFile(EntityDir + l_entityFile);
    if(!file_stream)
    {
        FAILURE_NON_FATAL("Could not open entity file {}", l_entityFile);
        return entity_id;
    }
    Utils::Tokens tokens{std::move(*file_stream)};
    
    while(!tokens.empty())
    {
        auto key = *consumeToken<std::string>(tokens);
        if(key == "Name")
        {

        }
        else if(key == "Attributes")
        {
            if(entity_id != -1) 
            {
                FAILURE_NON_FATAL("Invalid entity file {} : duplicate `Attributes` field found", l_entityFile);
                return -1;
            }
            entity_id = addEntity(*consumeToken<Bitset>(tokens));
        }
        else if(key == "Component")
        {
            if(entity_id == -1)
            {
                FAILURE_NON_FATAL("Invalid entity file {} : `Component` field found before `Attributes` field ", l_entityFile);
                return -1;
            }
            const auto CId = *consumeToken<unsigned int>(tokens);
            auto* component = getComponent<CBase>(entity_id, (Component) CId);
            if(component == nullptr)
            {
                FAILURE_NON_FATAL("Could not find component {}", CId);
                return -1;
            }
            component->readInput(tokens);
            if(auto* c_sprite_sheet = dynamic_cast<CSpriteSheet*>(component); component->getType() == Component::SpriteSheet)
            {
                c_sprite_sheet->create(m_textureManager);
            }
        }
    }

    return entity_id;
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
    auto new_component = itr2->second();
    itr->second.second.emplace_back(std::move(new_component));
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