#ifndef ECS_ENTITY_C_SPRITESHEET_HPP
#define ECS_ENTITY_C_SPRITESHEET_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <utils/utilities.hpp>
#include <ecs/entity/c_drawable.hpp>
#include <core/graphics/spriteSheet.hpp>
#include <optional>
#include <core/graphics/textureManager.fwd.hpp>

#include <string>

class CSpriteSheet : public CDrawable
{
public:
    CSpriteSheet();
    void readInput(Utils::Tokens& l_tokens) override;
    void create(TextureManager& l_textureManager, std::optional<std::string> l_sheetName = {});

    SpriteSheet* getSpriteSheet();
    
    void updatePosition(const sf::Vector2f& l_vec) override;
    const sf::Vector2u& getSize() override;
    void draw(sf::RenderWindow* l_window) override;

private:
    std::optional<SpriteSheet> m_spriteSheet;
    std::string m_sheetName;
};

#endif