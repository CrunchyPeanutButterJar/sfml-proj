#include <core/gui/GUI_scrollbar.hpp>
#include <core/gui/GUI_style.hpp>

#include <core/gui/GUI_element.hpp>
#include <core/gui/GUI_interface.hpp>
#include <core/gui/GUI_manager.hpp>
#include <utility>

using namespace core::gui;

GUI_Element::GUI_Element(std::string l_name, const GUI_ElementType& l_type, GUI_Interface* l_owner)
    : m_name(std::move(l_name)), m_type(l_type), m_owner(l_owner)
{
}

void GUI_Element::updateStyle(const GUI_ElementState& l_state, const GUI_Style& l_style)
{
    auto& textures = m_owner->getManager()->getContext()->m_textureManager;
    auto& fonts    = m_owner->getManager()->getContext()->m_fontManager;

    // Resource management.
    if (l_style.m_backgroundImage != m_style[l_state].m_backgroundImage)
    {
        m_stylesResources[l_state].m_backgroundImage = textures.acquire(l_style.m_backgroundImage);
    }

    if (l_style.m_glyph != m_style[l_state].m_glyph)
    {
        m_stylesResources[l_state].m_glyph = textures.acquire(l_style.m_glyph);
    }

    if (l_style.m_textFont != m_style[l_state].m_textFont)
    {
        m_stylesResources[l_state].m_font = fonts.acquire(l_style.m_textFont);
    }

    // Style application.
    m_style[l_state] = l_style;
    if (l_state == m_state)
    {
        setRedraw(true);
        applyStyle();
    }
}

void GUI_Element::applyStyle()
{
    applyTextStyle();
    applyBgStyle();
    applyGlyphStyle();
    if (m_owner != this && !isControl())
    {
        m_owner->adjustContentSize(this);
    }
}

void GUI_Element::applyTextStyle()
{
    auto&            fonts         = m_owner->getManager()->getContext()->m_fontManager;
    const GUI_Style& current_style = m_style[m_state];
    if (!current_style.m_textFont.empty())
    {
        m_visual.m_text.setFont(*fonts.acquire(current_style.m_textFont));
        m_visual.m_text.setFillColor(current_style.m_textColor);
        m_visual.m_text.setCharacterSize(current_style.m_textSize);
        if (current_style.m_textCenterOrigin)
        {
            sf::FloatRect rect = m_visual.m_text.getLocalBounds();
            m_visual.m_text.setOrigin(rect.left + rect.width / 2.0F, rect.top + rect.height / 2.0F);
        }
        else
        {
            m_visual.m_text.setOrigin(0.F, 0.F);
        }
    }
    m_visual.m_text.setPosition(m_position + current_style.m_textPadding);
}
void GUI_Element::applyBgStyle()
{
    auto&            textures      = m_owner->getManager()->getContext()->m_textureManager;
    const GUI_Style& current_style = m_style[m_state];
    if (!current_style.m_backgroundImage.empty())
    {
        m_visual.m_backgroundImage.setTexture(*textures.acquire(current_style.m_backgroundImage));
        m_visual.m_backgroundImage.setColor(current_style.m_backgroundImageColor);
    }
    m_visual.m_backgroundImage.setPosition(m_position);
    m_visual.m_backgroundSolid.setSize(sf::Vector2f(current_style.m_size));
    m_visual.m_backgroundSolid.setFillColor(current_style.m_backgroundColor);
    m_visual.m_backgroundSolid.setPosition(m_position);
}

void GUI_Element::applyGlyphStyle()
{
    const GUI_Style& current_style = m_style[m_state];
    auto&            textures      = m_owner->getManager()->getContext()->m_textureManager;
    if (!current_style.m_glyph.empty())
    {
        m_visual.m_glyph.setTexture(*textures.acquire(current_style.m_glyph));
    }
    m_visual.m_glyph.setPosition(m_position + current_style.m_glyphPadding);
}

auto GUI_Element::getType() -> GUI_ElementType
{
    return m_type;
}

void GUI_Element::setState(const GUI_ElementState& l_state)
{
    if (m_state == l_state)
    {
        return;
    }
    m_state = l_state;
    setRedraw(true);
}
auto GUI_Element::getName() const -> const std::string&
{
    return m_name;
}
void GUI_Element::setName(const std::string& l_name)
{
    m_name = l_name;
}
auto GUI_Element::getPosition() const -> const sf::Vector2f&
{
    return m_position;
}
void GUI_Element::setPosition(const sf::Vector2f& l_pos)
{
    m_position = l_pos;
    if (m_owner == nullptr || m_owner == this)
    {
        return;
    }
    const auto& padding = m_owner->getPadding();
    if (m_position.x < padding.x)
    {
        m_position.x = padding.x;
    }
    if (m_position.y < padding.y)
    {
        m_position.y = padding.y;
    }
}

auto GUI_Element::getSize() const -> const sf::Vector2f&
{
    return m_style.at(m_state).m_size;
}
auto GUI_Element::getState() const -> GUI_ElementState
{
    return m_state;
}
void GUI_Element::setRedraw(const bool& l_redraw)
{
    m_needsRedraw = l_redraw;
}
auto GUI_Element::needsRedraw() const -> bool
{
    return m_needsRedraw;
}
void GUI_Element::setOwner(GUI_Interface* l_owner)
{
    m_owner = l_owner;
}
auto GUI_Element::getOwner() const -> GUI_Interface*
{
    return m_owner;
}
auto GUI_Element::hasOwner() const -> bool
{
    return m_owner != nullptr;
}
auto GUI_Element::isActive() const -> bool
{
    return m_active;
}
void GUI_Element::setActive(const bool& l_active)
{
    if (l_active != m_active)
    {
        m_active = l_active;
        setRedraw(true);
    }
}
auto GUI_Element::isControl() const -> bool
{
    return m_isControl;
}
auto GUI_Element::getText() const -> std::string
{
    return m_visual.m_text.getString();
}
void GUI_Element::setText(const std::string& l_text)
{
    m_visual.m_text.setString(l_text);
    setRedraw(true);
}
auto GUI_Element::isInside(const sf::Vector2f& l_point) const -> bool
{
    sf::Vector2f position = getGlobalPosition();
    return (l_point.x >= position.x && l_point.y >= position.y &&
            l_point.x <= position.x + m_style.at(m_state).m_size.x &&
            l_point.y <= position.y + m_style.at(m_state).m_size.y);
}
auto GUI_Element::getGlobalPosition() const -> sf::Vector2f
{
    sf::Vector2f position = getPosition();
    if (m_owner == nullptr || m_owner == this)
    {
        return position;
    }
    position += m_owner->getGlobalPosition();
    if (isControl())
    {
        return position;
    }
    position.x -= m_owner->m_scrollHorizontal;
    position.y -= m_owner->m_scrollVertical;
    return position;
}