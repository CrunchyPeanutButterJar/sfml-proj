#ifndef ECS_ENTITY_C_BASE_HPP
#define ECS_ENTITY_C_BASE_HPP

#include <ecs/ecs_types.hpp>
#include <utils/utilities.hpp>
#include <memory>

namespace ecs::entity
{
class CBase
{
public:
    CBase(Component l_type) : m_type{l_type} {}
    virtual ~CBase() = default;

    Component getType() const {return m_type;}

    virtual void readInput(utils::Tokens&) = 0;

private:
    Component m_type;
};

using CBasePtr = std::unique_ptr<CBase>;
};

#endif