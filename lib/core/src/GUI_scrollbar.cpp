#include "utils/utilities.hpp"
#include <core/gui/GUI_interface.hpp>
#include <core/gui/GUI_manager.hpp>
#include <core/gui/GUI_scrollbar.hpp>
#include <core/shared_context.hpp>
#include <core/window.hpp>

using namespace core::gui;

GUI_Scrollbar::GUI_Scrollbar(const std::string& l_name, GUI_Interface* l_owner)
    : GUI_Element(l_name, GUI_ElementType::Scrollbar, l_owner)
{
    m_isControl = true;
}
GUI_Scrollbar::~GUI_Scrollbar() = default;

void GUI_Scrollbar::setPosition(const sf::Vector2f& l_pos)
{
    GUI_Element::setPosition(l_pos);
    if (m_sliderType == SliderType::Horizontal)
    {
        m_position.x = 0;
    }
    else
    {
        m_position.y = 0;
    }
}
void GUI_Scrollbar::readIn(utils::Tokens& l_tokens)
{
    m_sliderType = SliderType::Vertical;

    if (l_tokens.head<std::string>() == "Horizontal")
    {
        m_sliderType = SliderType::Horizontal;
        l_tokens.advance();
    }

    if (m_sliderType == SliderType::Horizontal)
    {
        m_slider.setPosition(0, getPosition().y);
    }
    else
    {
        m_slider.setPosition(getPosition().x, 0);
    }
}
void GUI_Scrollbar::onClick(const sf::Vector2f& l_mousePos)
{
    if (!m_slider.getGlobalBounds().contains(l_mousePos - m_owner->getPosition()))
    {
        return;
    }
    setState(GUI_ElementState::Clicked);
    m_moveMouseLast = l_mousePos;
}
void GUI_Scrollbar::onRelease()
{
    setState(GUI_ElementState::Neutral);
}
void GUI_Scrollbar::onHover(const sf::Vector2f& /*l_mousePos*/)
{
    setState(GUI_ElementState::Focused);
}
void GUI_Scrollbar::onLeave()
{
    setState(GUI_ElementState::Neutral);
}
void GUI_Scrollbar::updateStyle(const GUI_ElementState& l_state, const GUI_Style& l_style)
{
    GUI_Element::updateStyle(l_state, l_style);
    if (m_sliderType == SliderType::Horizontal)
    {
        m_style[l_state].m_size.x = m_owner->getSize().x;
    }
    else
    {
        m_style[l_state].m_size.y = m_owner->getSize().y;
    }
}

void GUI_Scrollbar::applyStyle()
{
    GUI_Element::applyStyle();
    m_slider.setFillColor(m_style[m_state].m_elementColor);
    bool  horizontal = m_sliderType == SliderType::Horizontal;
    auto& bg_solid   = m_visual.m_backgroundSolid;
    setPosition((horizontal ? sf::Vector2f(0, m_owner->getSize().y - bg_solid.getSize().y)
                            : sf::Vector2f(m_owner->getSize().x - bg_solid.getSize().x, 0)));
    bg_solid.setSize((horizontal ? sf::Vector2f(m_owner->getSize().x, m_style[m_state].m_size.y)
                                 : sf::Vector2f(m_style[m_state].m_size.x, m_owner->getSize().y)));
    m_slider.setPosition((horizontal ? m_slider.getPosition().x : getPosition().x),
                         (horizontal ? getPosition().y : m_slider.getPosition().y));
    float size_factor = (horizontal ? m_owner->getContentSize().x / m_owner->getSize().x
                                    : m_owner->getContentSize().y / m_owner->getSize().y);
    if (size_factor < 1.F)
    {
        size_factor = 1.F;
    }
    float slider_size = (horizontal ? m_owner->getSize().x : m_owner->getSize().y) / size_factor;
    m_slider.setSize((horizontal ? sf::Vector2f(slider_size, bg_solid.getSize().y)
                                 : sf::Vector2f(bg_solid.getSize().x, slider_size)));
    bg_solid.setPosition(getPosition());
}

void GUI_Scrollbar::update(float /*l_dT*/)
{
    // Mouse-drag code.
    if (getState() != GUI_ElementState::Clicked)
    {
        return;
    }
    SharedContext* context   = m_owner->getManager()->getContext();
    sf::Vector2f   mouse_pos = context->m_window.getMousePosition();
    if (m_moveMouseLast == mouse_pos)
    {
        return;
    }
    sf::Vector2f difference = mouse_pos - m_moveMouseLast;
    m_moveMouseLast         = mouse_pos;

    bool horizontal = m_sliderType == SliderType::Horizontal;
    m_slider.move((horizontal ? difference.x : 0), (horizontal ? 0 : difference.y));
    if (horizontal && m_slider.getPosition().x < 0)
    {
        m_slider.setPosition(0, m_slider.getPosition().y);
    }
    else if (m_slider.getPosition().y < 0)
    {
        m_slider.setPosition(m_slider.getPosition().x, 0);
    }
    if (horizontal && (m_slider.getPosition().x + m_slider.getSize().x > m_owner->getSize().x))
    {
        m_slider.setPosition(m_owner->getSize().x - m_slider.getSize().x, m_slider.getPosition().y);
    }
    else if (m_slider.getPosition().y + m_slider.getSize().y > m_owner->getSize().y)
    {
        m_slider.setPosition(m_slider.getPosition().x, m_owner->getSize().y - m_slider.getSize().y);
    }
    float work_area = (horizontal ? m_owner->getSize().x - m_slider.getSize().x
                                  : m_owner->getSize().y - m_slider.getSize().y);
    int   percentage =
        ((horizontal ? m_slider.getPosition().x : m_slider.getPosition().y) / work_area) * 100;
    if (horizontal)
    {
        m_owner->updateScrollHorizontal(percentage);
    }
    else
    {
        m_owner->updateScrollVertical(percentage);
    }
    setRedraw(true);
}
void GUI_Scrollbar::draw(sf::RenderTarget* l_target)
{
    l_target->draw(m_visual.m_backgroundSolid);
    l_target->draw(m_slider);
}