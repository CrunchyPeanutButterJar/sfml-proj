#ifndef CORE_RESOURCE_MANAGER_HPP
#define CORE_RESOURCE_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

namespace core
{
template <typename Derived, typename T> class ResourceManager
{
  public:
    ResourceManager(const std::string& l_pathFileName)
    {
        if (auto file_content = utils::readFile(utils::getConfigDirectory() + l_pathFileName))
        {
            utils::Tokens tokens{std::move(*file_content)}; // Alias Path

            while (!tokens.empty())
            {
                auto tuple = utils::consumeTokens<std::string, std::string>(tokens);
                ASSERT(tuple.has_value(), "Error reading from ressource file {}", l_pathFileName);

                const auto [alias, path] = *tuple;

                if (m_paths.find(alias) != m_paths.end())
                {
                    FAILURE_NON_FATAL("Duplicate alias {} found in file {}. Overriden value", alias,
                                      l_pathFileName);
                }
                m_paths[alias] = path;
            }

            return;
        }

        FAILURE("Could not read path file {}", l_pathFileName);
    }

    auto acquire(const std::string& l_alias) -> std::shared_ptr<T>
    {
        auto               it = m_resources.find(l_alias);
        std::shared_ptr<T> resource;

        if (it == m_resources.end() || it->second.lock() == nullptr)
        {
            auto resource_path_it = m_paths.find(l_alias);
            if (resource_path_it == m_paths.end())
            {
                FAILURE_NON_FATAL("Invalid resource alias {}", l_alias);
                return nullptr;
            }

            auto tmp = load(resource_path_it->second);
            if (tmp == nullptr)
            {
                FAILURE_NON_FATAL("Failed to load resource {} at {}", l_alias,
                                  resource_path_it->second);
                return nullptr;
            }

            resource = std::move(tmp);

            it = m_resources.insert_or_assign(l_alias, resource).first;
        }

        return it->second.lock();
    }

    static auto load(const std::string& l_path) -> std::unique_ptr<T>
    {
        return Derived::load(l_path);
    }

  private:
    std::unordered_map<std::string, std::string>      m_paths;     // alias - path mapping
    std::unordered_map<std::string, std::weak_ptr<T>> m_resources; // alias - resource mapping
};
} // namespace core
#endif