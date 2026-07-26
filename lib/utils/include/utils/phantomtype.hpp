#ifndef UTILS_PHANTOMTYPE_HPP
#define UTILS_PHANTOMTYPE_HPP

#include <utility>

namespace utils
{
template <typename UnderlyingType, typename PhantomTypeParam> class PhantomType
{
  public:
    using ReflectionType = UnderlyingType;

    template <typename... Args>
    explicit PhantomType(Args&&... args) : m_value{std::forward<Args>(args)...} {};

    auto get() -> UnderlyingType& { return m_value; }

    [[nodiscard]] auto get() const -> const UnderlyingType& { return m_value; }

    [[nodiscard]] auto reflection() const -> const ReflectionType& { return m_value; }

    operator UnderlyingType&() { return m_value; }

    operator const UnderlyingType&() const { return m_value; }

  private:
    UnderlyingType m_value;
};

template <typename UnderlyingType, typename PhantomTypeParam>
auto operator==(const PhantomType<UnderlyingType, PhantomTypeParam>& lhs,
                const PhantomType<UnderlyingType, PhantomTypeParam>& rhs) -> bool
{
    return lhs.get() == rhs.get();
}

} // namespace utils

#endif