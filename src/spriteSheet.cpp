#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <spriteSheet.hpp>
#include <animation.hpp>
#include <textureManager.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>
#include <utility>

SpriteSheet::SpriteSheet(TextureManager& l_textureManager) : m_textureManager(&l_textureManager) {}

bool SpriteSheet::loadSheet(const std::string& l_filePath)
{
    if(auto fstream = Utils::ReadFile(l_filePath))
    {
        Utils::Tokens tokens{std::move(*fstream)};

        while(!tokens.empty())
        {
            auto [key] = *Utils::ConsumeTokens<std::string>(tokens);
            if(key == "Size")
            {
                std::tie(m_spriteSize.x, m_spriteSize.y) = *Utils::ConsumeTokens<int, int>(tokens);
            }
            else if(key == "Scale")
            {
                std::tie(m_spriteScale.x, m_spriteScale.y) = *Utils::ConsumeTokens<float, float>(tokens);
            }
            else if(key == "AnimationsStart")
            {
                std::string animationType;
                while(std::tie(animationType) = *Utils::ConsumeTokens<std::string>(tokens), animationType != "AnimationsEnd")
                {
                    auto [animationName] = *Utils::ConsumeTokens<std::string>(tokens);
                    ASSERT(animationType == "Animation", "Invalid animation type {}", animationType);
                    auto animationPtr = std::make_unique<Animation>();
                    animationPtr->readInput(tokens);
                    auto [textureAlias] = *Utils::ConsumeTokens<std::string>(tokens);

                    auto texturePtr = m_textureManager->acquire(textureAlias);
                    ASSERT(texturePtr != nullptr, "Could not load texture {}", textureAlias);

                    animationPtr->m_spriteSheet = this;
                    animationPtr->m_texture = texturePtr.get();
                    animationPtr->m_name = animationName;
                    
                    m_animations.emplace(std::make_pair(animationName, std::move(animationPtr)));
                    setAnimation(animationName);
                    m_textures.push_back(std::move(texturePtr));
                }
            }
            else
            {
                FAILURE("Invalid Key {} read in config file {}", key, l_filePath);
            }
        }

        return true;
    }

    return false;
}

void SpriteSheet::setSpriteSize(const sf::Vector2i& l_size)
{
    m_spriteSize = l_size;
    m_sprite.setOrigin(m_spriteSize.x/2, m_spriteSize.y);
}

void SpriteSheet::setSpritePosition(const sf::Vector2f& l_pos)
{
    m_sprite.setPosition(l_pos);
}

void SpriteSheet::nextAnimation()
{
    static size_t current = 0;

    auto it = std::next(m_animations.begin(), current++%m_animations.size());
    setAnimation(it->first, true, false);
}

void SpriteSheet::setDirection(Direction l_dir)
{
    if(l_dir == m_direction)
    {
        return;
    }

    m_direction = l_dir;
    m_currentAnimation->cropSprite();
}

sf::Vector2i SpriteSheet::getSpriteSize() const
{
    return m_spriteSize;
}

sf::Vector2f SpriteSheet::getSpritePosition() const
{
    return m_sprite.getPosition();
}

Direction SpriteSheet::getDirection() const
{
    return m_direction;
}

BaseAnimation* SpriteSheet::getCurrentAnimation() const
{
    return m_currentAnimation;
}

void SpriteSheet::cropSprite(const sf::IntRect& l_rect)
{
    m_sprite.setTextureRect(l_rect);
}

bool SpriteSheet::setAnimation(const std::string& l_name, bool l_play, bool l_loop)
{
    auto itr = m_animations.find(l_name);
    const bool found = itr != m_animations.end();

    if (!found || itr->second.get() == m_currentAnimation)
    {
      ASSERT_NON_FATAL(found, "Attempt to set an invalid animation : {}", l_name);
      return false;
    }

    if (m_currentAnimation)
    {
        m_currentAnimation->stop();
    }

    m_currentAnimation = itr->second.get();
    m_sprite.setTexture(*m_currentAnimation->m_texture);

    m_currentAnimation->m_loop = l_loop;
    if(l_play)
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