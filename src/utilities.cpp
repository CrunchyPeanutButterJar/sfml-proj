#include <utilities.hpp>
#include <cctype>

namespace Utils
{

static std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) start++;

    auto end = s.end();
    do { end--; } while (end != start && std::isspace(*end));

    return std::string(start, end + 1);
}

std::vector<std::vector<std::string>> Tokenize(std::istringstream l_stream, char l_delimiter, char l_commentChar)
{
    std::vector<std::vector<std::string>> result;
    std::string line;

    while (std::getline(l_stream, line))
    {
        if (line.empty()) continue;

        if (line[0] == l_commentChar) continue;

        std::vector<std::string> tokens;
        std::string token;
        std::istringstream lineStream(line);

        while (std::getline(lineStream, token, l_delimiter))
        {
            token = trim(token);
            if (!token.empty()) tokens.push_back(token);
        }

        if (!tokens.empty()) result.push_back(tokens);
    }

    return result;
}

};