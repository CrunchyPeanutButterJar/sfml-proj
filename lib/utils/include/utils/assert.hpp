#ifndef UTILS_ASSERT_HPP
#define UTILS_ASSERT_HPP

#include <chrono>
#include <cstdlib>

#include <fmt/chrono.h>
#include <fmt/format.h>

#if defined(_MSC_VER)
#define DEBUG_BREAK() __debugbreak()
#elif defined(__clang__) || defined(__GNUC__)
#include <csignal>
#define DEBUG_BREAK() raise(SIGTRAP)
#endif

namespace utils::internal
{
inline void vlog(FILE* f, const char* file, const char* function, int line, fmt::string_view fmt,
                 fmt::format_args args)
{
    fmt::print(f, "[INFO] [{:%H:%M:%S}] {}:{} #{} {}\n", std::chrono::system_clock::now(), file,
               line, function, fmt::vformat(fmt, args));
}

template <typename... T>
void log(FILE* f, const char* file, const char* function, int line, fmt::format_string<T...> fmt,
         T&&... args)
{
    vlog(f, file, function, line, fmt, fmt::make_format_args(args...));
}

inline void assertionFailedVlog(const char* file, const char* function, int line,
                                fmt::string_view fmt, fmt::format_args args)
{
    fmt::print(stderr, "[ERROR] [{:%H:%M:%S}] {}:{} #{} {}\n", std::chrono::system_clock::now(),
               file, line, function, fmt::vformat(fmt, args));
}

template <typename... T>
void assertionFailedLog(const char* file, const char* function, int line,
                        fmt::format_string<T...> fmt, T&&... args)
{
    assertionFailedVlog(file, function, line, fmt, fmt::make_format_args(args...));
}

template <bool critical, typename... Args>
inline void ensureImpl(bool condition, const char* file, const char* function, int line,
                       fmt::format_string<Args...> fmt, Args&&... args)
{
    if (!condition)
    {
        assertionFailedLog(file, function, line, fmt, args...);
        if constexpr (critical)
        {
            DEBUG_BREAK();
            exit(EXIT_FAILURE);
        }
    }
}
} // namespace utils::internal

#define ASSERT_NON_FATAL(condition, ...)                                                           \
    utils::internal::ensureImpl<false>((condition), __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define ASSERT(condition, ...)                                                                     \
    utils::internal::ensureImpl<true>((condition), __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#define FAILURE_NON_FATAL(...) ASSERT_NON_FATAL(false, ##__VA_ARGS__)

#define FAILURE(...) ASSERT(false, ##__VA_ARGS__)

#define LOG(...) utils::internal::log(stdout, __FILE__, __func__, __LINE__, ##__VA_ARGS__)

#ifdef DEBUG_BUILD
#define ASSERT_DEBUG_BUILD(condition, ...) ASSERT(condition, ##__VA_ARGS__)
#else
#define ASSERT_DEBUG_BUILD(condition, ...) ((void)0)
#endif

#ifdef DEBUG_BUILD
#define LOG_DEBUG(...) LOG(__VA_ARGS__)
#else
#define LOG_DEBUG(...) ((void)0)
#endif

#endif