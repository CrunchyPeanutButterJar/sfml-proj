#include <core/font_manager.hpp>
#include <utils/utilities.hpp>

namespace core
{
FontManager::FontManager() : ResourceManager{"fonts.cfg"} {}

auto FontManager::load(const std::string& l_path) -> std::unique_ptr<sf::Font>
{
    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(utils::getResourcesDirectory() + l_path))
    {
        font.reset();
    }
    return font;
}
} // namespace core