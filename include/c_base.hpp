#ifndef C_BASE_HPP
#define C_BASE_HPP

#include <ecs_types.hpp>
#include <utils/utilities.hpp>
#include <memory>

class C_Base
{
public:
    C_Base(Component l_type) : m_type{l_type} {}
    virtual ~C_Base() = default;

    Component getType() const {return m_type;}

    virtual void readInput(Utils::Tokens&) = 0;

private:
    Component m_type;
};

using C_BasePtr = std::unique_ptr<C_Base>;

#endif