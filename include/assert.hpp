#ifndef ASSERT_HPP
#define ASSERT_HPP

#include <stdlib.h>
#include <string>
#include <tuple>

#include <fmt/format.h>

inline void _vlog(FILE* f, const char* file, int line, fmt::string_view fmt, fmt::format_args args)
{
    fmt::print(f, "[LOG] {}:{}: {}\n", file, line, fmt::vformat(fmt, args));
}

template <typename... T>
void _log(FILE* f, const char* file, int line, fmt::format_string<T...> fmt, T&&... args)
{
    _vlog(f, file, line, fmt, fmt::make_format_args(args...));
}

inline void _assertion_failed_vlog(const char* file, int line, fmt::string_view fmt, fmt::format_args args) 
{
    fmt::print(stderr, "[ERROR]: {}:{}: {}\n", file, line, fmt::vformat(fmt, args));
}

template <typename... T>
void _assertion_failed_log(const char* file, int line, fmt::format_string<T...> fmt, T&&... args)
{
  _assertion_failed_vlog(file, line, fmt, fmt::make_format_args(args...));
}

template<bool critical, typename... Args>
inline void ensure_impl(bool condition, const char* file, int line, fmt::format_string<Args...> fmt, Args&&... args)
{
    if (!condition)
    {
        _assertion_failed_log(file, line, fmt, args...);
        if constexpr (critical)
        {
            exit(EXIT_FAILURE);
        }
    }
}

#define LOG_ERROR(condition, ...) ensure_impl<false>((condition), __FILE__, __LINE__, ##__VA_ARGS__)

#define ASSERT(condition, ...) ensure_impl<true>((condition), __FILE__, __LINE__, ##__VA_ARGS__)

#define FAILURE(...) _assertion_failed_log(__FILE__, __LINE__, ##__VA_ARGS__), exit(EXIT_FAILURE)

#define LOG(...) _log(stdout, __FILE__, __LINE__, ##__VA_ARGS__)

#ifdef DEBUG_BUILD
#define ENSURE(condition, ...) ensure_impl<true>((condition), __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define ENSURE(condition, ...) ((void)0) // No-op in release builds
#endif

#endif