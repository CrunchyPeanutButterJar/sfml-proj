#ifndef UTILS_PHANTOMTYPE_HPP
#define UTILS_PHANTOMTYPE_HPP

#include <utility>

template<typename UnderlyingType, typename PhantomTypeParam>
class PhantomType
{
public:
    using ReflectionType = UnderlyingType;

    template<typename... Args>
    explicit PhantomType(Args&&... args) : m_value{std::forward<Args>(args)...} {};

    UnderlyingType& get()
    {
        return m_value;
    }

    const UnderlyingType& get() const
    {
        return m_value;
    }

    const ReflectionType& reflection() const
    {
        return m_value;
    }

    operator UnderlyingType&()
    {
        return m_value;
    }

    operator const UnderlyingType&() const
    {
        return m_value;
    }

private:
    UnderlyingType m_value;
};

#endif