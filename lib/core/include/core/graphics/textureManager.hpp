#ifndef CORE_GRAPHICS_TEXTUREMANAGER_HPP
#define CORE_GRAPHICS_TEXTUREMANAGER_HPP

#include <core/graphics/textureManager.fwd.hpp>
#include <core/resourceManager.hpp>
#include <SFML/Graphics/Texture.hpp>

class TextureManager : public ResourceManager<TextureManager, sf::Texture>
{
public:
    TextureManager();

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    
    TextureManager(TextureManager&&) = default;
    TextureManager& operator=(TextureManager&&) = default;


    static std::unique_ptr<sf::Texture> load(const std::string &l_path);
};

#endif