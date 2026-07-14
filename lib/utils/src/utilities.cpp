#include <cctype>
#include <fstream>
#include <sstream>
#include <utils/utilities.hpp>

namespace utils
{

Tokens::Tokens(std::istringstream ss, char l_delimiter, char l_commentChar)
    : m_ss{std::move(ss)}, m_delimiter{l_delimiter}, m_commentChar{l_commentChar}
{
}

auto Tokens::currentMatch() -> bool
{
    constexpr auto Ltrim = [](std::string& s, const std::string& chars = " \t\n\r")
    {
        s.erase(0, s.find_first_not_of(chars));
    };

    constexpr auto Rtrim = [](std::string& s, const std::string& chars = " \t\n\r")
    {
        if (auto pos = s.find_last_not_of(chars); pos != std::string::npos)
            s.erase(pos + 1);
    };

    constexpr auto Read = [](auto& l_ss, const std::string& l_delimiters) -> std::string
    {
        std::string token;
        char        c = 0;
        while (l_ss.get(c))
        {
            token.push_back(c);
            if (l_delimiters.contains(c))
            {
                break;
            }
        }

        return token;
    };

    if (!m_currentStr.empty())
    {
        return true;
    }

    if (m_ss.eof())
    {
        return false;
    }

    m_currentStr = Read(m_ss, {m_delimiter, '\n'}); // new line always a delimiter
    Ltrim(m_currentStr);

    if (m_quoteChar && !m_currentStr.empty() && m_currentStr[0] == m_quoteChar.value())
    {
        auto str     = Read(m_ss, {m_quoteChar.value()});
        m_currentStr = m_currentStr.substr(1) + std::move(str);
        if (m_currentStr.back() == m_quoteChar.value())
        {
            m_currentStr.pop_back(); // remove quote char
        }
        else
        {
            FAILURE_NON_FATAL("quoted string not closed properly {} with {}", m_currentStr,
                              m_quoteChar.value());
        }
        return currentMatch();
    }

    if (m_currentStr.back() == m_delimiter)
    {
        m_currentStr.pop_back();
    }

    Rtrim(m_currentStr);

    if (!m_currentStr.empty() && m_currentStr[0] == m_commentChar)
    {
        std::getline(m_ss, m_currentStr, '\n');
        m_currentStr.clear();
    }
    return currentMatch();
}

void Tokens::skipLine()
{
    if (currentMatch())
    {
        std::getline(m_ss, m_currentStr, '\n');
        m_currentStr.clear();
    }
}

auto Tokens::empty() -> bool
{
    return !currentMatch();
}

auto Tokens::advance() -> std::optional<std::string>
{
    if (currentMatch())
    {
        std::string result = std::move(m_currentStr);
        m_currentStr.clear();
        return result;
    }

    return std::nullopt;
}

void Tokens::captureQuotedStrings(std::optional<char> l_quoteChar)
{
    m_quoteChar = l_quoteChar;
}

auto readFile(const std::string& l_filePath) -> std::optional<std::istringstream>
{
    std::ifstream file{l_filePath};
    if (!file)
    {
        return {};
    }

    std::string file_content{std::istreambuf_iterator<char>(file),
                             std::istreambuf_iterator<char>()};

    return std::istringstream{std::move(file_content)};
}

auto pgcd(size_t l_n1, size_t l_n2) -> size_t
{
    size_t n1 = std::max(l_n1, l_n2);
    size_t n2 = std::min(l_n1, l_n2);

    size_t r = n1 % n2;

    if (r == 0)
    {
        return n2;
    }

    return pgcd(n2, r);
}

auto ppcm(size_t l_n1, size_t l_n2) -> size_t
{
    return l_n1 * l_n2 / pgcd(l_n1, l_n2);
}
} // namespace utils