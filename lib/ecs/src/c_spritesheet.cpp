#include <ecs/entity/c_spritesheet.hpp>

#include <utils/utilities.hpp>
#include <utils/assert.hpp>
#include <core/graphics/textureManager.hpp>

using namespace ecs::entity;

CSpriteSheet::CSpriteSheet():
CDrawable(Component::SpriteSheet)
{}

void CSpriteSheet::readInput(utils::Tokens& l_tokens)
{
    m_sheetName = *consumeToken<std::string>(l_tokens);
}

void CSpriteSheet::create(core::graphics::TextureManager& l_textureManager, std::optional<std::string> l_sheetName)
{
    if(m_spriteSheet)
    {
        LOG("Overriding spritesheet");
    }

    m_spriteSheet.emplace(core::graphics::SpriteSheet{l_textureManager});
    m_spriteSheet->loadSheet(utils::getResourcesDirectory() + "media/spritesheets/" + (l_sheetName? *l_sheetName: m_sheetName));
}

core::graphics::SpriteSheet* CSpriteSheet::getSpriteSheet()
{
    if(m_spriteSheet)
    {
        return &m_spriteSheet.value();
    }

    return nullptr;
}

void CSpriteSheet::updatePosition(const sf::Vector2f& l_position)
{
    ASSERT(m_spriteSheet.has_value(), "core::graphics::SpriteSheet {} was not loaded!", m_sheetName);
    m_spriteSheet->setSpritePosition(l_position);
}

const sf::Vector2u& CSpriteSheet::getSize()
{
    ASSERT(m_spriteSheet.has_value(), "core::graphics::SpriteSheet {} was not loaded!", m_sheetName);
    return m_spriteSheet->getSpriteSize();
}

void CSpriteSheet::draw(sf::RenderWindow* l_window)
{
    ASSERT(m_spriteSheet.has_value(), "core::graphics::SpriteSheet {} was not loaded!", m_sheetName);
    ASSERT(l_window != nullptr, "RenderWindow is nullptr!");
    m_spriteSheet->draw(l_window);
}