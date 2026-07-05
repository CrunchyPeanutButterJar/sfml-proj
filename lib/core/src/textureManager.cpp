#include <SFML/Graphics/Texture.hpp>
#include <core/graphics/textureManager.hpp>
#include <utils/utilities.hpp>

using namespace core::graphics;

TextureManager::TextureManager() : ResourceManager{"textures.cfg"} {}

std::unique_ptr<sf::Texture> TextureManager::load(const std::string& l_path)
{
    auto texture = std::make_unique<sf::Texture>();
    if(!texture->loadFromFile(utils::getResourcesDirectory() + l_path))
    {
        texture.reset();
    }

    return texture;
}