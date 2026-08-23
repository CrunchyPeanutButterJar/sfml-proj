#ifndef UTILS_RANDOM_HPP
#define UTILS_RANDOM_HPP

#include <random>
#include <type_traits>

namespace utils
{
template <typename T> auto getRandom(T l_min, T l_max) -> T
{
    static std::mt19937 s_gen(std::random_device{}());
    if constexpr (std::is_integral_v<T>)
    {
        std::uniform_int_distribution<T> dist(l_min, l_max);
        return dist(s_gen);
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        std::uniform_real_distribution<T> dist(l_min, l_max);
        return dist(s_gen);
    }
    else
    {
        static_assert(false, "Unsupported type");
        throw;
    }
}
} // namespace utils

#endif