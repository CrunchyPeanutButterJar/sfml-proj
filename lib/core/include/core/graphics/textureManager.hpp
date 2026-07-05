#ifndef CORE_GRAPHICS_TEXTUREMANAGER_HPP
#define CORE_GRAPHICS_TEXTUREMANAGER_HPP

#include <core/graphics/textureManager.fwd.hpp>
#include <core/resourceManager.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace core::graphics
{
class TextureManager : public ResourceManager<TextureManager, sf::Texture> {
public:
    TextureManager();

    TextureManager(const TextureManager&) = delete;
    auto operator=(const TextureManager&) -> TextureManager& = delete;
    
    TextureManager(TextureManager&&) = default;
    auto operator=(TextureManager&&) -> TextureManager& = default;

    ~TextureManager() = default;


    static auto load(const std::string &l_path) -> std::unique_ptr<sf::Texture>;
};
} // namespace core::graphics
#endif