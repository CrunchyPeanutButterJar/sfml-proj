#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <core/animation/animation.hpp>
#include <core/graphics/spritesheet.hpp>
#include <core/graphics/texture_manager.hpp>
#include <utility>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

using namespace core::graphics;

SpriteSheet::SpriteSheet(core::graphics::TextureManager& l_textureManager)
    : m_textureManager(&l_textureManager)
{
}

auto SpriteSheet::loadSheet(const std::string& l_filePath) -> bool
{
    if (auto fstream = utils::readFile(l_filePath))
    {
        utils::Tokens tokens{std::move(*fstream)};

        while (!tokens.empty())
        {
            auto [key] = *utils::consumeTokens<std::string>(tokens);
            if (key == "Size")
            {
                std::tie(m_spriteSize.x, m_spriteSize.y) =
                    *utils::consumeTokens<unsigned int, unsigned int>(tokens);
            }
            else if (key == "Scale")
            {
                std::tie(m_spriteScale.x, m_spriteScale.y) =
                    *utils::consumeTokens<float, float>(tokens);
            }
            else if (key == "AnimationsStart")
            {
                std::string animation_type;
                while (std::tie(animation_type) = *utils::consumeTokens<std::string>(tokens),
                       animation_type != "AnimationsEnd")
                {
                    auto [animationName] = *utils::consumeTokens<std::string>(tokens);
                    ASSERT(animation_type == "Animation", "Invalid animation type {}",
                           animation_type);
                    auto animation_ptr = std::make_unique<core::animation::Animation>();
                    animation_ptr->readInput(tokens);
                    auto [textureAlias] = *utils::consumeTokens<std::string>(tokens);

                    auto texture_ptr = m_textureManager->acquire(textureAlias);
                    ASSERT(texture_ptr != nullptr, "Could not load texture {}", textureAlias);

                    animation_ptr->m_spriteSheet = this;
                    animation_ptr->m_texture     = texture_ptr.get();
                    animation_ptr->m_name        = animationName;

                    m_animations.emplace(std::make_pair(animationName, std::move(animation_ptr)));
                    setAnimation(animationName);
                    m_textures.push_back(std::move(texture_ptr));
                }
            }
            else
            {
                FAILURE("Invalid Key {} read in config file {}", key, l_filePath);
            }
        }
        setSpriteSize(m_spriteSize);

        return true;
    }

    return false;
}

void SpriteSheet::setSpriteSize(const sf::Vector2u& l_size)
{
    m_spriteSize = l_size;
    m_sprite.setOrigin(m_spriteSize.x / 2, m_spriteSize.y);
}

void SpriteSheet::setSpritePosition(const sf::Vector2f& l_pos)
{
    m_sprite.setPosition(l_pos);
}

void SpriteSheet::nextAnimation()
{
    static size_t current = 0;

    auto it = std::next(m_animations.begin(), current++ % m_animations.size());
    setAnimation(it->first, true, false);
}

void SpriteSheet::setDirection(core::Direction l_dir)
{
    if (l_dir == m_direction)
    {
        return;
    }

    m_direction = l_dir;
    m_currentAnimation->cropSprite();
}

auto SpriteSheet::getSpriteSize() const -> const sf::Vector2u&
{
    return m_spriteSize;
}

auto SpriteSheet::getSpritePosition() const -> const sf::Vector2f&
{
    return m_sprite.getPosition();
}

auto SpriteSheet::getDirection() const -> core::Direction
{
    return m_direction;
}

auto SpriteSheet::getCurrentAnimation() const -> core::animation::BaseAnimation*
{
    return m_currentAnimation;
}

void SpriteSheet::cropSprite(const sf::IntRect& l_rect)
{
    m_sprite.setTextureRect(l_rect);
    m_sprite.setScale(m_spriteScale);
}

auto SpriteSheet::setAnimation(const std::string& l_name, bool l_play, bool l_loop) -> bool
{
    auto       itr   = m_animations.find(l_name);
    const bool Found = itr != m_animations.end();

    if (!Found || itr->second.get() == m_currentAnimation)
    {
        ASSERT_NON_FATAL(Found, "Attempt to set an invalid animation : {}", l_name);
        return false;
    }

    if (m_currentAnimation != nullptr)
    {
        m_currentAnimation->stop();
    }

    m_currentAnimation = itr->second.get();
    m_sprite.setTexture(*m_currentAnimation->m_texture);

    m_currentAnimation->m_loop = l_loop;
    if (l_play)
    {
        m_currentAnimation->play();
    }
    m_currentAnimation->cropSprite();

    return true;
}

void SpriteSheet::update(float l_dt)
{
    m_currentAnimation->update(l_dt);
}

void SpriteSheet::draw(sf::RenderWindow* l_window)
{
    l_window->draw(m_sprite);
}