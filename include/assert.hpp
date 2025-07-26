#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <tuple>

template<bool critical, typename... Args>
inline void ensure_impl(bool condition, const char* file, int line, Args&&... args)
{
    if (!condition)
    {
        fprintf(stderr, "Assertion failed: %s:%d:", file, line);
        if constexpr (sizeof...(args) > 0)
        { 
            using FirstArgType = std::decay_t<decltype((std::get<0>(std::forward_as_tuple(args...))))>;
            static_assert(std::is_same_v<FirstArgType, const char*>, "Formatted string must be a const char*");
            fprintf(stderr, args...);
        }
        fprintf(stderr, "\n");

        if constexpr (critical)
        {
            exit(EXIT_FAILURE);
        }
    }
}

#define LOG_ERROR(condition, ...) ensure_impl<false>((condition), __FILE__, __LINE__, ##__VA_ARGS__)

#define ASSERT(condition, ...) ensure_impl<true>((condition), __FILE__, __LINE__, ##__VA_ARGS__)

#ifdef DEBUG_BUILD
#define ENSURE(condition, ...) ensure_impl<true>((condition), __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define ENSURE(condition, ...) ((void)0) // No-op in release builds
#endif

#endif