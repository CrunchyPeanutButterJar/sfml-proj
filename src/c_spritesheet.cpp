#include <c_spritesheet.hpp>

#include <utils/utilities.hpp>
#include <utils/assert.hpp>
#include <textureManager.hpp>

C_SpriteSheet::C_SpriteSheet():
C_Drawable(Component::SpriteSheet)
{}

void C_SpriteSheet::readInput(Utils::Tokens& l_tokens)
{
    m_sheetName = *ConsumeToken<std::string>(l_tokens);
}

void C_SpriteSheet::create(TextureManager& l_textureManager, std::optional<std::string> l_sheetName)
{
    if(m_spriteSheet)
    {
        LOG("Overriding spritesheet");
    }

    m_spriteSheet.emplace(SpriteSheet{l_textureManager});
    m_spriteSheet->loadSheet(Utils::GetResourcesDirectory() + "media/spritesheets/" + (l_sheetName? *l_sheetName: m_sheetName));
}

SpriteSheet* C_SpriteSheet::getSpriteSheet()
{
    if(m_spriteSheet)
    {
        return &m_spriteSheet.value();
    }

    return nullptr;
}

void C_SpriteSheet::updatePosition(const sf::Vector2f& l_position)
{
    ASSERT(m_spriteSheet.has_value(), "SpriteSheet {} was not loaded!", m_sheetName);
    m_spriteSheet->setSpritePosition(l_position);
}

const sf::Vector2u& C_SpriteSheet::getSize()
{
    ASSERT(m_spriteSheet.has_value(), "SpriteSheet {} was not loaded!", m_sheetName);
    return m_spriteSheet->getSpriteSize();
}

void C_SpriteSheet::draw(sf::RenderWindow* l_window)
{
    ASSERT(m_spriteSheet.has_value(), "SpriteSheet {} was not loaded!", m_sheetName);
    ASSERT(l_window != nullptr, "RenderWindow is nullptr!");
    m_spriteSheet->draw(l_window);
}