#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <assert.hpp>
#include <utilities.hpp>
#include <memory>
#include <unordered_map>

template<typename Derived, typename T>
class ResourceManager
{
public:

ResourceManager(const std::string& l_pathFileName)
{
    if(auto fileContent = Utils::ReadFile(Utils::GetWorkingDirectory() + l_pathFileName))
    {
        auto tokens = Utils::Tokenize(std::move(*fileContent)); // Alias Path

        for(const auto& line : tokens)
        {
            const auto [alias, path] = Utils::ReadTokens<std::string, std::string>(line);
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

    if(it == m_resources.end() || it->second->lock() == nullptr)
    {
        auto resourcePathIt = m_paths.find(l_alias);
        if (resourcePathIt == m_paths.end())
        {
            LOG("Invalid resource alias {}", l_alias);
            return nullptr;
        }

        auto tmp = load(resourcePathIt->second);
        if(tmp == nullptr)
        {
            LOG("Failed to load resource {} at {}", l_alias, resourcePathIt->second);
            return nullptr;
        }

        resource = std::move(tmp);

        it = m_resources.insert_or_assign(l_alias, resource);
    }

    return it->second;
}

protected:
std::unique_ptr<T> load(const std::string& l_path)
{
    return static_cast<Derived*>(this)->load(l_path);
}

private:
    std::unordered_map<std::string, std::string> m_paths;//alias - path mapping
    std::unordered_map<std::string, std::weak_ptr<T>> m_resources;//alias - resource mapping

};

#endif