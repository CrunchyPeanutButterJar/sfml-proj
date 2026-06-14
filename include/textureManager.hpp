#ifndef TEXTUREMANAGER_HPP
#define TEXTUREMANAGER_HPP

#include <textureManager.fwd.hpp>
#include <resourceManager.hpp>
#include <SFML/Graphics/Texture.hpp>

class TextureManager : public ResourceManager<TextureManager, sf::Texture>
{
public:
    TextureManager();

protected:
    std::unique_ptr<sf::Texture> load(const std::string &l_path);
};

#endif