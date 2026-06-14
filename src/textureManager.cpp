#include <SFML/Graphics/Texture.hpp>
#include <textureManager.hpp>
#include <utilities.hpp>

TextureManager::TextureManager() : ResourceManager{"resources/textures.cfg"} {}

std::unique_ptr<sf::Texture> TextureManager::load(const std::string& l_path)
{
    auto texture = std::make_unique<sf::Texture>();
    if(!texture->loadFromFile(Utils::GetWorkingDirectory() + l_path))
    {
        texture.reset();
    }

    return texture;
}