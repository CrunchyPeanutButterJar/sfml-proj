#include <SFML/Graphics/Texture.hpp>
#include <core/graphics/textureManager.hpp>
#include <utils/utilities.hpp>

TextureManager::TextureManager() : ResourceManager{"textures.cfg"} {}

std::unique_ptr<sf::Texture> TextureManager::load(const std::string& l_path)
{
    auto texture = std::make_unique<sf::Texture>();
    if(!texture->loadFromFile(Utils::getResourcesDirectory() + l_path))
    {
        texture.reset();
    }

    return texture;
}