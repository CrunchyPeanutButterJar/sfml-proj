#include <sstream>
#include <utils/utilities.hpp>
#include <cctype>
#include <fstream>

namespace Utils
{

Tokens::Tokens(std::istringstream ss, char l_delimiter, char l_commentChar) : m_ss{std::move(ss)}, m_delimiter{l_delimiter}, m_commentChar{l_commentChar} {}

bool Tokens::currentMatch()
{
    constexpr auto Trim =
    [](std::string& s, const std::string& chars = " \t\n\r") 
    {
        s.erase(0, s.find_first_not_of(chars));//ltrim
        if (auto pos = s.find_last_not_of(chars); pos != std::string::npos) s.erase(pos + 1);//rtrim
    };

    constexpr auto Read = 
    [](auto& l_ss, const std::string& l_delimiters) -> std::string
    {
        std::string token;
        char c = 0;
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
    
    m_currentStr = Read(m_ss, {m_delimiter, '\n'});//new line always a delimiter
    Trim(m_currentStr);
    if(!m_currentStr.empty() && m_currentStr[0] == m_commentChar)
    {
        std::getline(m_ss, m_currentStr, '\n');
        m_currentStr.clear();
    }
    return currentMatch();
}

void Tokens::skipLine()
{
    if(currentMatch())
    {
        std::getline(m_ss, m_currentStr, '\n');
        m_currentStr.clear();
    }
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

std::optional<std::istringstream> readFile(const std::string& l_filePath)
{
    std::ifstream file{l_filePath};
    if (!file)
    {
        return {};
    }
    
    std::string file_content{std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()};

    return std::istringstream{std::move(file_content)};
}

size_t pgcd(size_t l_n1, size_t l_n2)
{
    size_t n1 = std::max(l_n1, l_n2);
    size_t n2 = std::min(l_n1, l_n2);

    size_t r = n1 % n2;

    if(r == 0)
    {
        return n2;
    }

    return pgcd(n2, r);
}

size_t ppcm(size_t l_n1, size_t l_n2)
{
    return l_n1 * l_n2 / pgcd(l_n1, l_n2);
}
};