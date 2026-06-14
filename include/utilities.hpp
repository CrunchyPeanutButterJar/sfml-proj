#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <assert.hpp>
#include <string>
#include <sstream>
#include <vector>
#include <optional>

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

    std::optional<std::istringstream> readFile(const std::string& l_fileName);
    std::vector<std::vector<std::string>> Tokenize(std::istringstream l_stream, char l_delimiter = ' ', char l_commentChar = '#');
};

#endif