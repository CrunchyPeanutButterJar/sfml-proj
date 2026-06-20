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
    std::tuple<T...> ConsumeTokens(Tokens& l_tokens)
    {
      static constexpr auto ToType = 
      []<typename Type>(const std::string& l_token, Type*) -> Type
      {
        static const std::string typeName = printTypeName<Type>();

        std::istringstream iss(l_token);
        Type value;
        iss >> value;
        ASSERT_NON_FATAL(!iss.fail(), "Failed to parse token str {} to type {}", l_token, typeName);
        return value;
      };

      static constexpr auto ConsumeToken = 
      [] (Tokens& l_tokens)
      {
		auto token = l_tokens.advance();
		ASSERT(token.has_value(), "Failed to consume token");
		return token.value();
      };

      static constexpr auto ToTuple =
      [] (Tokens& l_tokens) -> std::tuple<T...>
      {
        return std::make_tuple(ToType(ConsumeToken(l_tokens), (T*) nullptr)...);
      };

      auto tuple = ToTuple(l_tokens);

      return tuple;
    }
};

#endif