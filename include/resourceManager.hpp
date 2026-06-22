#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <utils/assert.hpp>
#include <utils/utilities.hpp>
#include <memory>
#include <unordered_map>

template<typename Derived, typename T>
class ResourceManager
{
public:

ResourceManager(const std::string& l_pathFileName)
{
    if(auto fileContent = Utils::ReadFile(Utils::GetConfigDirectory() + l_pathFileName))
    {
        Utils::Tokens tokens{std::move(*fileContent)}; // Alias Path

        while(!tokens.empty())
        {
            auto tuple = Utils::ConsumeTokens<std::string, std::string>(tokens);
            ASSERT(tuple.has_value(), "Error reading from ressource file {}", l_pathFileName);

            const auto [alias, path] = *tuple;
            
            if(m_paths.find(alias) != m_paths.end())
            {
                FAILURE_NON_FATAL("Duplicate alias {} found in file {}. Overriden value", alias, l_pathFileName);
            }
            m_paths[alias] = path;
        }

        return;
    }

    FAILURE("Could not read path file {}", l_pathFileName);
}

std::shared_ptr<T> acquire(const std::string& l_alias)
{
    auto it = m_resources.find(l_alias);
    std::shared_ptr<T> resource;

    if(it == m_resources.end() || it->second.lock() == nullptr)
    {
        auto resourcePathIt = m_paths.find(l_alias);
        if (resourcePathIt == m_paths.end())
        {
            FAILURE_NON_FATAL("Invalid resource alias {}", l_alias);
            return nullptr;
        }

        auto tmp = load(resourcePathIt->second);
        if(tmp == nullptr)
        {
            FAILURE_NON_FATAL("Failed to load resource {} at {}", l_alias, resourcePathIt->second);
            return nullptr;
        }

        resource = std::move(tmp);

        it = m_resources.insert_or_assign(l_alias, resource).first;
    }

    return it->second.lock();
}

std::unique_ptr<T> load(const std::string& l_path)
{
    return static_cast<Derived*>(this)->load(l_path);
}

private:
    std::unordered_map<std::string, std::string> m_paths;//alias - path mapping
    std::unordered_map<std::string, std::weak_ptr<T>> m_resources;//alias - resource mapping

};

#endif