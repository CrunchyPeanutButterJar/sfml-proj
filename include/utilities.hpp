#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <assert.hpp>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <optional>
#include <tuple>

namespace Utils
{
    #ifdef _WIN32
    #include <windows.h>
    #include <Shlwapi.h>
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
    inline std::string GetWorkingDirectory()
    {
        char cwd[1024];
        if(getcwd(cwd, sizeof(cwd)) != nullptr)
        {
            return std::string(cwd) + std::string("/");
        }
        FAILURE("There should not a be problem here...");
    }
    #endif

    std::optional<std::istringstream> ReadFile(const std::string& l_filePath);
    std::vector<std::vector<std::string>> Tokenize(std::istringstream l_stream, char l_delimiter = ' ', char l_commentChar = '#');
    
    template<typename... T>
    std::tuple<T...> ReadTokens(const std::vector<std::string>& l_tokens)
    {
      static constexpr auto ToType = 
      []<typename Type>(const std::string& l_token, Type*) -> Type
      {
        std::istringstream iss(l_token);
        Type value;
        iss >> value;
        return value;
      };

      static constexpr auto ToTuple =
      [] <std::size_t... Is>(const std::vector<std::string>& l_tokens, std::index_sequence<Is...>) -> std::tuple<T...>
      {
        return std::make_tuple(ToType(l_tokens[Is], (T*) nullptr)...);
      };

      return ToTuple(l_tokens, std::make_index_sequence<sizeof...(T)>());
    }

    template <typename T>
    T ReadTokenKey(const std::vector<std::string>& l_tokens)
    {
      auto [key] = ReadTokens<T>(l_tokens);
      return key;
    }

    template <typename... T>
    std::tuple<T...> ReadTokenValues(std::vector<std::string> l_tokens)
    {
      l_tokens.erase(l_tokens.begin());
      return ReadTokens<T...>(l_tokens); 
    }
};

#endif