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
        return {};
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

    class Tokens
    {
    public:
        Tokens(std::istringstream ss, char l_delimiter = ' ', char l_commentChar = '#');

        std::optional<std::string> advance();
		    bool empty();

    private:
        bool currentMatch();

		std::string m_currentStr;
		std::istringstream m_ss;
		const char m_delimiter;
		const char m_commentChar;
    };

    template<typename... T>
    std::optional<std::tuple<T...>> ConsumeTokens(Tokens& l_tokens)
    {
      bool error = false;

      static constexpr auto ToType = 
      []<typename Type>(const std::string& l_token, bool& l_error, Type*) -> Type
      {
        static const std::string typeName = printTypeName<Type>();

        std::istringstream iss(l_token);
        Type value;
        iss >> value;
        if(iss.fail())
        {
          FAILURE_NON_FATAL( "Failed to parse token str {} to type {}", l_token, typeName);
          l_error = true;
        }
        return value;
      };

      static constexpr auto ConsumeToken = 
      [] (Tokens& l_tokens, bool& l_error)
      {
        auto token = l_tokens.advance();
        if(!token.has_value())
        {
          FAILURE_NON_FATAL( "Failed to consume token");
          l_error = true;
        }
        return token.value();
      };

      static constexpr auto ToTuple =
      [] (Tokens& l_tokens, bool& l_error) -> std::tuple<T...>
      {
        return std::make_tuple(ToType(ConsumeToken(l_tokens, l_error), l_error, (T*) nullptr)...);
      };

      auto tuple = ToTuple(l_tokens, error);
      if(!error)
      {
        return tuple;
      }
      
      return std::nullopt;
    }
};

#endif