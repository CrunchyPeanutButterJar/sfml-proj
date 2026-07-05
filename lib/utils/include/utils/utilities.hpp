#ifndef UTILS_UTILITIES_HPP
#define UTILS_UTILITIES_HPP

#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <utils/assert.hpp>

namespace utils
{
#ifdef _WIN32
#include <Shlwapi.h>
#include <windows.h>
inline std::string GetWorkingDirectory()
{
    HMODULE hModule = GetModuleHandle(nullptr);
    if (hModule)
    {
        char path[256];
        GetModuleFileName(hModule, path, sizeof(path));
        PathRemoveFileSpec(path);
        strcat_s(path, "\\");     // new
        return std::string(path); // new
    }
    return "";
}
#elif __linux__
#include <unistd.h>
inline auto getWorkingDirectory() -> std::string
{
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr)
    {
        return std::string(cwd) + std::string("/");
    }
    FAILURE("There should not a be problem here...");
    return {};
}
#endif

inline auto getConfigDirectory() -> std::string
{
    return getWorkingDirectory() + "config/";
}

inline auto getResourcesDirectory() -> std::string
{
    return getWorkingDirectory() + "resources/";
}

template <typename T> auto printTypeName() -> std::string
{
    return __PRETTY_FUNCTION__;
}

auto readFile(const std::string& l_filePath) -> std::optional<std::istringstream>;

class Tokens
{
  public:
    Tokens(std::istringstream ss, char l_delimiter = ' ', char l_commentChar = '#');

    auto advance() -> std::optional<std::string>;
    auto empty() -> bool;
    void skipLine();

    template <typename T> auto head() -> std::optional<T>
    {
        if (!currentMatch())
        {
            return std::nullopt;
        }
        std::istringstream iss(m_currentStr);
        T                  value;
        iss >> value;
        if (iss.fail())
        {
            return std::nullopt;
        }
        return value;
    }

  private:
    auto currentMatch() -> bool;

    std::string        m_currentStr;
    std::istringstream m_ss;
    const char         m_delimiter;
    const char         m_commentChar;
};

template <typename... T> auto consumeTokens(Tokens& l_tokens) -> std::optional<std::tuple<T...>>
{
    bool error = false;

    static constexpr auto ToType = []<typename Type>(const std::string& l_token, bool& l_error,
                                                     Type*) -> Type
    {
        static const std::string TypeName = printTypeName<Type>();

        std::istringstream iss(l_token);
        Type               value;
        iss >> value;
        if (iss.fail())
        {
            FAILURE_NON_FATAL("Failed to parse token str {} to type {}", l_token, TypeName);
            l_error = true;
        }
        return value;
    };

    static constexpr auto ConsumeToken = [](Tokens& l_tokens, bool& l_error) -> std::string
    {
        auto token = l_tokens.advance();
        if (!token.has_value())
        {
            FAILURE_NON_FATAL("Failed to consume token");
            l_error = true;
            return "";
        }
        return token.value();
    };

    static constexpr auto ToTuple = [](Tokens& l_tokens, bool& l_error) -> std::tuple<T...>
    { return std::make_tuple(ToType(ConsumeToken(l_tokens, l_error), l_error, (T*)nullptr)...); };

    auto tuple = ToTuple(l_tokens, error);
    if (!error)
    {
        return tuple;
    }

    return std::nullopt;
}

template <typename T> auto consumeToken(Tokens& l_tokens) -> std::optional<T>
{
    if (auto tuple = consumeTokens<T>(l_tokens))
    {
        return std::get<0>(*tuple);
    }

    return std::nullopt;
}

auto ppcm(size_t n1, size_t n2) -> size_t;

auto pgcd(size_t n1, size_t n2) -> size_t;
} // namespace utils

#endif