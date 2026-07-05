#include <algorithm>
#include <ecs/system/s_base.hpp>

using namespace ecs;
using namespace ecs::system;
using namespace ecs::messaging;

SBase::SBase(System l_id, SystemManager& l_sysManager) : m_systemManager{l_sysManager}, m_id{l_id}
{
}

auto SBase::addEntity(EntityId l_entity) -> bool
{
    if (hasEntity(l_entity))
    {
        return false;
    }
    m_entities.emplace_back(l_entity);
    return true;
}

auto SBase::hasEntity(EntityId l_entity) -> bool
{
    return std::find(m_entities.begin(), m_entities.end(), l_entity) != m_entities.end();
}

auto SBase::removeEntity(EntityId l_entity) -> bool
{
    auto itr = std::find(m_entities.begin(), m_entities.end(), l_entity);
    if (itr == m_entities.end())
    {
        return false;
    }
    m_entities.erase(itr);
    return true;
}

auto SBase::getId() const -> System
{
    return m_id;
}

auto SBase::fitsRequirements(utils::Bitmask l_bits) -> bool
{
    return std::any_of(m_requiredComponents.begin(), m_requiredComponents.end(),
                       [l_bits](const utils::Bitmask& l_requirement)
                       { return l_requirement.matches(l_bits, l_bits.getMask()); });
}