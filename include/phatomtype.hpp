#ifndef PHANTOMTYPE_HPP
#define PHANTOMTYPE_HPP

template<typename UnderlyingType, typename PhantomTypeParam>
class PhantomType
{
public:
    template<typename... Args>
    PhantomType(Args&&... args) : m_value{std::forward<Args>(args)...} {};

    UnderlyingType& get()
    {
        return m_value;
    }

    const UnderlyingType& get() const
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