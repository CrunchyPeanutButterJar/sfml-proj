#ifndef UTILS_UTILITIES_HPP
#define UTILS_UTILITIES_HPP

#include <algorithm>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <utils/assert.hpp>
#include <utils/utilities.fwd.hpp>

#ifdef _WIN32
#include <Shlwapi.h>
#include <windows.h>
#elif __linux__
#include <unistd.h>
#endif

namespace utils
{
#ifdef _WIN32
inline auto getWorkingDirectory() -> std::string
{
    HMODULE hModule = GetModuleHandle(nullptr);
    if (hModule)
    {
        char path[256];
        GetModuleFileName(hModule, path, sizeof(path));
        PathRemoveFileSpec(path);
        std::replace(path, path + sizeof(path), '\\', '/');
        strcat_s(path, "/");      // new
        return std::string(path); // new
    }
    return "";
}
#elif __linux__
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

  private:
    char                m_delimiter;
    std::optional<char> m_quoteChar;

    template <typename T, T Tokens::*Member> class ScopedReplacer
    {
      public:
        ScopedReplacer(Tokens& l_tokens, T l_newMemberValue)
            : m_tokens{l_tokens}, m_oldMemberValue{m_tokens.*Member}
        {
            l_tokens.*Member = l_newMemberValue;
        }

        ~ScopedReplacer() { m_tokens.*Member = m_oldMemberValue; }

      private:
        Tokens& m_tokens;
        T       m_oldMemberValue;
    };

  public:
    auto setDelimiterScoped(char l_delimiter) -> ScopedReplacer<char, &Tokens::m_delimiter>;
    auto setQuotedCharScoped(std::optional<char> l_quoteChar)
        -> ScopedReplacer<std::optional<char>, &Tokens::m_quoteChar>;

    template <typename T> auto head() -> std::optional<T>
    {
        if (!currentMatch())
        {
            return std::nullopt;
        }

        if constexpr (std::is_same_v<T, std::string>)
        {
            return m_currentStr;
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
    const char         m_commentChar;
};

template <typename... T> auto consumeTokens(Tokens& l_tokens) -> std::optional<std::tuple<T...>>
{
    bool error = false;

    static constexpr auto ToType = []<typename Type>(const std::string& l_token, bool& l_error,
                                                     Type*) -> Type
    {
        static const std::string TypeName = printTypeName<Type>();

        if constexpr (std::is_same_v<Type, std::string>)
        {
            return l_token;
        }

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
    {
        std::tuple<T...> result;

        constexpr auto Assign =
            [](auto self, Tokens& l_tokens, bool& l_error, auto& first, auto&... rest)
        {
            first = ToType(ConsumeToken(l_tokens, l_error), l_error, &first);
            if constexpr (sizeof...(rest) > 0)
                self(self, l_tokens, l_error, rest...);
        };

        std::apply([&](auto&... args) { Assign(Assign, l_tokens, l_error, args...); }, result);

        return result;
    };

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