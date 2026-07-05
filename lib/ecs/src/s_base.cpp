#include <ecs/system/s_base.hpp>
#include <algorithm>

SBase::SBase(System l_id, SystemManager& l_sysManager):
m_systemManager{l_sysManager},
m_id{l_id} {}

bool SBase::addEntity(EntityId l_entity)
{
    if(hasEntity(l_entity))
    {
        return false;
    }
    m_entities.emplace_back(l_entity);
    return true;
}

bool SBase::hasEntity(EntityId l_entity)
{
    return std::find(m_entities.begin(), m_entities.end(), l_entity) != m_entities.end();
}

bool SBase::removeEntity(EntityId l_entity)
{
    auto itr = std::find(m_entities.begin(), m_entities.end(), l_entity);
    if(itr == m_entities.end())
    {
        return false;
    }
    m_entities.erase(itr);
    return true;
}

System SBase::getId() const
{
    return m_id;
}

bool SBase::fitsRequirements(Bitmask l_bits)
{
    return std::any_of(m_requiredComponents.begin(), m_requiredComponents.end(),
    [l_bits](const Bitmask& l_requirement)
    {
        return l_requirement.matches(l_bits, l_bits.getMask());
    });
}