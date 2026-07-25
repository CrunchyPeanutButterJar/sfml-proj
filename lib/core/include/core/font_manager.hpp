#ifndef CORE_FONT_MANAGER_HPP
#define CORE_FONT_MANAGER_HPP

#include <SFML/Graphics/Font.hpp>
#include <core/font_manager.fwd.hpp>
#include <core/resource_manager.hpp>

namespace core
{
class FontManager : public ResourceManager<FontManager, sf::Font>
{
  public:
    FontManager();

    static auto load(const std::string& l_path) -> std::unique_ptr<sf::Font>;
};
} // namespace core
#endif
