#include <sstream>
#include <utilities/utilities.hpp>
#include <cctype>
#include <fstream>

namespace Utils
{

Tokens::Tokens(std::istringstream ss, char l_delimiter, char l_commentChar) : m_ss{std::move(ss)}, m_delimiter{l_delimiter}, m_commentChar{l_commentChar} {}

bool Tokens::currentMatch()
{
    constexpr auto trim =
    [](std::string& s, const std::string& chars = " \t\n\r") 
    {
        s.erase(0, s.find_first_not_of(chars));//ltrim
        if (auto pos = s.find_last_not_of(chars); pos != std::string::npos) s.erase(pos + 1);//rtrim
    };

    constexpr auto read = 
    [](auto& l_ss, const std::string& l_delimiters) -> std::string
    {
        std::string token;
        char c;
        while(l_ss.get(c) && !l_delimiters.contains(c))
        {
            token.push_back(c);
        }

        return token;
    };

    if(!m_currentStr.empty())
    {
        return true;
    }

    if(m_ss.eof())
    {
        return false;
    }
    
    m_currentStr = read(m_ss, {m_delimiter, '\n'});//new line always a delimiter
    trim(m_currentStr);
    if(!m_currentStr.empty() && m_currentStr[0] == m_commentChar)
    {
        std::getline(m_ss, m_currentStr, '\n');
        m_currentStr.clear();
    }
    return currentMatch();
}

bool Tokens::empty()
{
    return !currentMatch();
}

std::optional<std::string> Tokens::advance()
{
    if(currentMatch())
    {
        std::string result = std::move(m_currentStr);
        m_currentStr.clear();
        return result;
    }

    return std::nullopt;
}

std::optional<std::istringstream> ReadFile(const std::string& l_filePath)
{
    std::ifstream file{l_filePath};
    if (!file)
    {
        return {};
    }
    
    std::string fileContent{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

    return std::istringstream{std::move(fileContent)};
}

};