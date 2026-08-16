#include <SFML/Graphics/Color.hpp>
#include <ecs/entity/c_spritesheet.hpp>

#include <SFML/Graphics/RectangleShape.hpp>

#include <core/graphics/texture_manager.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

using namespace ecs::entity;

CSpriteSheet::CSpriteSheet() : CDrawable(Component::SpriteSheet) {}

void CSpriteSheet::readInput(utils::Tokens& l_tokens)
{
    m_sheetName = *consumeToken<std::string>(l_tokens);
}

void CSpriteSheet::create(core::graphics::TextureManager& l_textureManager,
                          std::optional<std::string>      l_sheetName)
{
    if (m_spriteSheet)
    {
        LOG("Overriding spritesheet");
    }

    m_spriteSheet.emplace(core::graphics::SpriteSheet{l_textureManager});
    ASSERT_NON_FATAL(m_spriteSheet->loadSheet(utils::getResourcesDirectory() +
                                              "media/spritesheets/" +
                                              (l_sheetName ? *l_sheetName : m_sheetName)),
                     "Could not load spritesheet");
}

auto CSpriteSheet::getSpriteSheet() -> core::graphics::SpriteSheet*
{
    if (m_spriteSheet)
    {
        return &m_spriteSheet.value();
    }

    return nullptr;
}

void CSpriteSheet::updatePosition(const sf::Vector2f& l_position)
{
    ASSERT(m_spriteSheet.has_value(), "core::graphics::SpriteSheet {} was not loaded!",
           m_sheetName);
    m_spriteSheet->setSpritePosition(l_position);
}

auto CSpriteSheet::getSize() -> const sf::Vector2u&
{
    ASSERT(m_spriteSheet.has_value(), "core::graphics::SpriteSheet {} was not loaded!",
           m_sheetName);
    return m_spriteSheet->getSpriteSize();
}

bool CSpriteSheet::debug_overlay = false;

void CSpriteSheet::draw(sf::RenderWindow* l_window)
{
    ASSERT(m_spriteSheet.has_value(), "core::graphics::SpriteSheet {} was not loaded!",
           m_sheetName);
    ASSERT(l_window != nullptr, "RenderWindow is nullptr!");
    m_spriteSheet->draw(l_window);

    if (debug_overlay)
    {
        const auto [Width, Height] = m_spriteSheet->getSpriteSize();
        sf::RectangleShape outline{{(float)Width, (float)Height}};
        outline.setOutlineColor(sf::Color::Red);
        outline.setOutlineThickness(3.);
        outline.setFillColor(sf::Color::Transparent);

        outline.setOrigin((float)Width - (float)Width / 2., (float)Height);

        outline.setPosition(m_spriteSheet->getSpritePosition());

        l_window->draw(outline);
    }
}