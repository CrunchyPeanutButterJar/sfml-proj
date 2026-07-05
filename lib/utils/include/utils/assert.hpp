#ifndef UTILS_ASSERT_HPP
#define UTILS_ASSERT_HPP

#include <stdlib.h>

#include <fmt/format.h>

namespace utils::internal
{
inline void vlog(FILE* f, const char* file, const char* function, int line, fmt::string_view fmt, fmt::format_args args)
{
    fmt::print(f, "[INFO] {}:{} #{} {}\n", file, line, function, fmt::vformat(fmt, args));
}

template <typename... T>
void log(FILE* f, const char* file, const char* function, int line, fmt::format_string<T...> fmt, T&&... args)
{
    vlog(f, file, function, line, fmt, fmt::make_format_args(args...));
}

inline void assertionFailedVlog(const char* file, const char* function, int line, fmt::string_view fmt, fmt::format_args args) 
{
    fmt::print(stderr, "[ERROR]: {}:{} #{} {}\n", file, line, function, fmt::vformat(fmt, args));
}

template <typename... T>
void assertionFailedLog(const char* file,const char* function, int line, fmt::format_string<T...> fmt, T&&... args)
{
  assertionFailedVlog(file, function, line, fmt, fmt::make_format_args(args...));
}

template<bool critical, typename... Args>
inline void ensureImpl(bool condition, const char* file, const char* function, int line, fmt::format_string<Args...> fmt, Args&&... args)
{
    if (!condition)
    {
        assertionFailedLog(file, function, line, fmt, args...);
        if constexpr (critical)
        {
            exit(EXIT_FAILURE);
        }
    }
}
}

#define ASSERT_NON_FATAL(condition, ...) utils::internal::ensureImpl<false>((condition), __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define ASSERT(condition, ...) utils::internal::ensureImpl<true>((condition), __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define FAILURE_NON_FATAL(...) ASSERT_NON_FATAL(false, ##__VA_ARGS__)

#define FAILURE(...) ASSERT(false, ##__VA_ARGS__)

#define LOG(...) utils::internal::log(stdout, __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#ifdef DEBUG_BUILD
#define ASSERT_DEBUG_BUILD(condition, ...) ASSERT(condition, ##__VA_ARGS__)
#else
#define ASSERT_DEBUG_BUILD(condition, ...) ((void)0) // No-op in release builds
#endif

#endif