#ifndef CORE_GRAPHICS_SPRITESHEET_HPP
#define CORE_GRAPHICS_SPRITESHEET_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/directions.hpp>
#include <core/graphics/textureManager.fwd.hpp>
#include <core/animation/baseAnimation.hpp>

#include <unordered_map>
#include <memory>

namespace core::graphics
{
using AnimationPtr = std::unique_ptr<core::animation::BaseAnimation>;
using Animations =  std::unordered_map<std::string, AnimationPtr>;

class SpriteSheet
{
public:
    SpriteSheet(core::graphics::TextureManager& l_textureManager);

    bool loadSheet(const std::string& l_filePath);

    void cropSprite(const sf::IntRect& l_rect);

    bool setAnimation(const std::string& l_name, bool l_play = false, bool l_Loop = false);
    void nextAnimation();//temp for testing purposes

    void setSpriteSize(const sf::Vector2u& l_size); 
    void setSpritePosition(const sf::Vector2f& l_pos);
    void setDirection(core::Direction l_dir);

    core::animation::BaseAnimation *getCurrentAnimation() const;
    const sf::Vector2u& getSpriteSize() const; 
    const sf::Vector2f& getSpritePosition() const;
    core::Direction getDirection() const;

    void update(float l_dt);
    void draw(sf::RenderWindow* l_window);

private:
    sf::Sprite m_sprite;
    sf::Vector2u m_spriteSize;
    sf::Vector2f m_spriteScale{1.F, 1.F};
    core::Direction m_direction{core::Direction::Right};
    Animations m_animations;
    core::animation::BaseAnimation* m_currentAnimation{nullptr};
    core::graphics::TextureManager* m_textureManager;
    std::vector<std::shared_ptr<sf::Texture>> m_textures;
};
};

#endif