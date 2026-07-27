#include <core/gui/GUI_label.hpp>
#include <utils/utilities.hpp>

using namespace core::gui;

GUI_Label::GUI_Label(const std::string& l_name, GUI_Interface* l_owner)
    : GUI_Element(l_name, GUI_ElementType::Label, l_owner)
{
}
GUI_Label::~GUI_Label() = default;

void GUI_Label::readIn(utils::Tokens& l_tokens)
{
    l_tokens.captureQuotedStrings('"');
    auto content = *consumeToken<std::string>(l_tokens);
    l_tokens.captureQuotedStrings({});
    m_visual.m_text.setString(content);
}

void GUI_Label::onClick(const sf::Vector2f& /*l_mousePos*/)
{
    setState(GUI_ElementState::Clicked);
}
void GUI_Label::onRelease()
{
    setState(GUI_ElementState::Neutral);
}
void GUI_Label::onHover(const sf::Vector2f& /*l_mousePos*/)
{
    setState(GUI_ElementState::Focused);
}
void GUI_Label::onLeave()
{
    setState(GUI_ElementState::Neutral);
}
void GUI_Label::update(float /*l_dT*/) {}
void GUI_Label::draw(sf::RenderTarget* l_target)
{
    l_target->draw(m_visual.m_backgroundSolid);
    if (!m_style[m_state].m_glyph.empty())
    {
        l_target->draw(m_visual.m_glyph);
    }
    if (!m_style[m_state].m_backgroundImage.empty())
    {
        l_target->draw(m_visual.m_backgroundImage);
    }
    l_target->draw(m_visual.m_text);
}