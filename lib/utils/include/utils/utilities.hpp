#ifndef UTILS_UTILITIES_HPP
#define UTILS_UTILITIES_HPP

#include <utils/assert.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <optional>
#include <tuple>

namespace utils
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
    inline std::string getWorkingDirectory()
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

    inline std::string getConfigDirectory()
    {
      return getWorkingDirectory() + "config/";
    }

    inline std::string getResourcesDirectory()
    {
      return getWorkingDirectory() + "resources/";
    }

    template<typename T>
    std::string printTypeName()
    {
      return __PRETTY_FUNCTION__;
    }

    std::optional<std::istringstream> readFile(const std::string& l_filePath);

    class Tokens
    {
    public:
        Tokens(std::istringstream ss, char l_delimiter = ' ', char l_commentChar = '#');

        std::optional<std::string> advance();
		    bool empty();
        void skipLine();

        template<typename T>
        std::optional<T> head()
        {
          if(!currentMatch())
          {
            return std::nullopt;
          }
          std::istringstream iss(m_currentStr);
          T value;
          iss >> value;
          if(iss.fail())
          {
            return std::nullopt;
          }
          return value;
        }

    private:
        bool currentMatch();

		std::string m_currentStr;
		std::istringstream m_ss;
		const char m_delimiter;
		const char m_commentChar;
    };

    template<typename... T>
    std::optional<std::tuple<T...>> consumeTokens(Tokens& l_tokens)
    {
      bool error = false;

      static constexpr auto ToType = 
      []<typename Type>(const std::string& l_token, bool& l_error, Type*) -> Type
      {
        static const std::string TypeName = printTypeName<Type>();

        std::istringstream iss(l_token);
        Type value;
        iss >> value;
        if(iss.fail())
        {
          FAILURE_NON_FATAL( "Failed to parse token str {} to type {}", l_token, TypeName);
          l_error = true;
        }
        return value;
      };

      static constexpr auto ConsumeToken = 
      [] (Tokens& l_tokens, bool& l_error) -> std::string
      {
        auto token = l_tokens.advance();
        if(!token.has_value())
        {
          FAILURE_NON_FATAL( "Failed to consume token");
          l_error = true;
          return "";
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

    template<typename T>
    std::optional<T> consumeToken(Tokens& l_tokens)
    {
      if(auto tuple = consumeTokens<T>(l_tokens))
      {
        return std::get<0>(*tuple);
      }

      return std::nullopt;
    }

    size_t ppcm(size_t n1, size_t n2);
    
    size_t pgcd(size_t n1, size_t n2);
};

#endif