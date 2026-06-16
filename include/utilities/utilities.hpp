#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <utilities/assert.hpp>
#include <string>
#include <sstream>
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

    inline std::string GetConfigDirectory()
    {
      return GetWorkingDirectory() + "config/";
    }

    inline std::string GetResourcesDirectory()
    {
      return GetWorkingDirectory() + "resources/";
    }

    template<typename T>
    std::string printTypeName()
    {
      return __PRETTY_FUNCTION__;
    }

    std::optional<std::istringstream> ReadFile(const std::string& l_filePath);
    std::vector<std::vector<std::string>> Tokenize(std::istringstream l_stream, char l_delimiter = ' ', char l_commentChar = '#');
    
    template<typename... T>
    std::tuple<T...> ConsumeTokens(std::vector<std::string>& l_tokens)
    {
      std::reverse(l_tokens.begin(), l_tokens.end());

      static constexpr auto ToType = 
      []<typename Type>(const std::string& l_token, Type*) -> Type
      {
        static const std::string typeName = printTypeName<Type>();

        std::istringstream iss(l_token);
        Type value;
        iss >> value;
        LOG_ERROR(!iss.fail(), "Failed to parse token str {} to type {}", l_token, typeName);
        return value;
      };

      static constexpr auto ConsumeBack = 
      [] (std::vector<std::string>& l_vector)
      {
        auto back = l_vector.back();
        l_vector.pop_back();
        return back;
      };

      static constexpr auto ToTuple =
      [] (std::vector<std::string>& l_tokens) -> std::tuple<T...>
      {
        return std::make_tuple(ToType(ConsumeBack(l_tokens), (T*) nullptr)...);
      };

      auto tuple = ToTuple(l_tokens);

      std::reverse(l_tokens.begin(), l_tokens.end());

      return tuple;
    }

    template<typename... T>
    std::tuple<T...> ConsumeTokens(std::vector<std::string>&& l_tokens)
    {
      return ConsumeTokens<T...>(l_tokens);
    }

    template <typename... T>
    std::tuple<T...> ConsumeTokens(std::vector<std::vector<std::string>>& l_tokensCollection) 
    {
      auto& tokens = l_tokensCollection[0];
      auto tuple = ConsumeTokens<T...>(tokens);
      if(tokens.empty())
      {
        l_tokensCollection.erase(l_tokensCollection.begin());
      }
      return tuple;
    }
};

#endif