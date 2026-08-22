#include "utils/utilities.hpp"
#include <SFML/System/Vector2.hpp>
#include <core/gui/GUI_interface.hpp>
#include <core/gui/GUI_manager.hpp>
#include <core/shared_context.hpp>
#include <core/window.hpp>

using namespace core::gui;

GUI_Interface::GUI_Interface(const std::string& l_name, GUI_Manager* l_guiManager)
    : GUI_Element(l_name, GUI_ElementType::Window, this), m_guiManager(l_guiManager)

{
    m_backdropTexture = std::make_unique<sf::RenderTexture>();
    m_contentTexture  = std::make_unique<sf::RenderTexture>();
    m_controlTexture  = std::make_unique<sf::RenderTexture>();
}

GUI_Interface::~GUI_Interface() = default;

void GUI_Interface::setPosition(const sf::Vector2f& l_pos)
{
    GUI_Element::setPosition(l_pos);
    m_backdrop.setPosition(l_pos);
    m_content.setPosition(l_pos);
    m_control.setPosition(l_pos);
    m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
    m_visual.m_text.setPosition(m_titleBar.getPosition() + m_style[m_state].m_textPadding);
}

auto GUI_Interface::addElement(const GUI_ElementType& l_type, const std::string& l_name) -> bool
{
    if (m_elements.find(l_name) != m_elements.end())
    {
        return false;
    }
    GUI_ElementPtr element{};
    element = m_guiManager->createElement(l_type, this);
    if (!element)
    {
        return false;
    }
    element->setName(l_name);
    element->setOwner(this);
    m_elements.emplace(l_name, std::move(element));
    m_contentRedraw = true;
    m_controlRedraw = true;
    return true;
}

auto GUI_Interface::getElement(const std::string& l_name) const -> GUI_Element*
{
    auto itr = m_elements.find(l_name);
    return (itr != m_elements.end() ? itr->second.get() : nullptr);
}

auto GUI_Interface::removeElement(const std::string& l_name) -> bool
{
    auto itr = m_elements.find(l_name);
    if (itr == m_elements.end())
    {
        return false;
    }
    m_elements.erase(itr);
    m_contentRedraw = true;
    m_controlRedraw = true;
    adjustContentSize();
    return true;
}

auto GUI_Interface::hasParent() const -> bool
{
    return m_parent != nullptr;
}
auto GUI_Interface::getManager() const -> GUI_Manager*
{
    return m_guiManager;
}

auto core::gui::getOriginalPoint(sf::Vector2f l_point, GUI_Manager* l_guiManager) -> sf::Vector2f
{
    return core::getOriginalPoint(l_point, l_guiManager->getContext()->m_window);
}

auto GUI_Interface::isInside(const sf::Vector2f& l_point) const -> bool
{
    const auto OriginalMousePos = getOriginalPoint(l_point, m_guiManager);

    if (GUI_Element::isInside(OriginalMousePos))
    {
        return true;
    }
    return m_titleBar.getGlobalBounds().contains(OriginalMousePos);
}

void GUI_Interface::focus()
{
    m_focused = true;
}
void GUI_Interface::defocus()
{
    m_focused = false;
}
auto GUI_Interface::isFocused() const -> bool
{
    return m_focused;
}

void GUI_Interface::readIn(utils::Tokens& l_tokens)
{
    std::string title;

    std::tie(m_elementPadding.x, m_elementPadding.y) = *consumeTokens<float, float>(l_tokens);

    if (*l_tokens.head<std::string>() == "Movable")
    {
        m_movable = true;
        l_tokens.advance();
    }
    if (*l_tokens.head<std::string>() == "Title")
    {
        m_showTitleBar = true;
        l_tokens.advance();
    }

    auto scope = l_tokens.setDelimiterScoped('"');
    title      = *consumeToken<std::string>(l_tokens);

    m_visual.m_text.setString(title);
}

void GUI_Interface::onClick(const sf::Vector2f& l_mousePos)
{
    defocusTextfields();
    const auto OriginalMousePos = getOriginalPoint(l_mousePos, m_guiManager);

    if (m_titleBar.getGlobalBounds().contains(OriginalMousePos) && m_movable && m_showTitleBar)
    {
        m_beingMoved = true;
    }
    else
    {
        GUI_Event event{};
        event.m_type            = GUI_EventType::Click;
        event.m_interface       = m_name.c_str();
        event.m_element         = "";
        event.m_clickCoords.m_x = l_mousePos.x;
        event.m_clickCoords.m_y = l_mousePos.y;
        m_guiManager->addEvent(event);

        for (auto& itr : m_elements)
        {
            if (!itr.second->isInside(OriginalMousePos))
            {
                continue;
            }
            itr.second->onClick(l_mousePos);
            event.m_element = itr.second->m_name.c_str();
            m_guiManager->addEvent(event);
        }
        setState(GUI_ElementState::Clicked);
    }
}

void GUI_Interface::onRelease()
{
    GUI_Event event{};
    event.m_type      = GUI_EventType::Release;
    event.m_interface = m_name.c_str();
    event.m_element   = "";
    m_guiManager->addEvent(event);
    for (auto& itr : m_elements)
    {
        if (itr.second->getState() != GUI_ElementState::Clicked)
        {
            continue;
        }
        itr.second->onRelease();
        event.m_element = itr.second->m_name.c_str();
        m_guiManager->addEvent(event);
    }
    setState(GUI_ElementState::Neutral);
}

void GUI_Interface::onHover(const sf::Vector2f& l_mousePos)
{
    GUI_Event event{};
    event.m_type            = GUI_EventType::Hover;
    event.m_interface       = m_name.c_str();
    event.m_element         = "";
    event.m_clickCoords.m_x = l_mousePos.x;
    event.m_clickCoords.m_y = l_mousePos.y;
    m_guiManager->addEvent(event);

    setState(GUI_ElementState::Focused);
}

void GUI_Interface::onLeave()
{
    GUI_Event event{};
    event.m_type      = GUI_EventType::Leave;
    event.m_interface = m_name.c_str();
    event.m_element   = "";
    m_guiManager->addEvent(event);

    setState(GUI_ElementState::Neutral);
}

void GUI_Interface::onTextEntered(const char& l_char)
{
    for (auto& itr : m_elements)
    {
        if (itr.second->getType() != GUI_ElementType::Textfield)
        {
            continue;
        }
        if (itr.second->getState() != GUI_ElementState::Clicked)
        {
            continue;
        }
        if (l_char == 8)
        {
            // Backspace.
            const auto& text = itr.second->getText();
            itr.second->setText(text.substr(0, text.length() - 1));
            return;
        }
        if (l_char < 32 || l_char > 126)
        {
            return;
        }
        std::string text = itr.second->getText();
        text.push_back(l_char);
        itr.second->setText(text);
        return;
    }
}

auto GUI_Interface::getPadding() const -> const sf::Vector2f&
{
    return m_elementPadding;
}
void GUI_Interface::setPadding(const sf::Vector2f& l_padding)
{
    m_elementPadding = l_padding;
}

void GUI_Interface::update(float l_dT)
{
    const auto MousePos = m_guiManager->getContext()->m_window.getMousePosition();

    if (m_beingMoved && m_moveMouseLast != MousePos)
    {
        const auto   OriginalMousePos = getOriginalPoint(MousePos, m_guiManager);
        sf::Vector2f difference =
            OriginalMousePos - getOriginalPoint(m_moveMouseLast, m_guiManager);
        m_moveMouseLast           = MousePos;
        sf::Vector2f new_position = m_position + difference;
        setPosition(new_position);
    }

    for (auto& itr : m_elements)
    {
        if (itr.second->needsRedraw())
        {
            if (itr.second->isControl())
            {
                m_controlRedraw = true;
            }
            else
            {
                m_contentRedraw = true;
            }
        }
        if (!itr.second->isActive())
        {
            continue;
        }
        itr.second->update(l_dT);
        if (m_beingMoved)
        {
            continue;
        }
        GUI_Event event{};
        event.m_interface       = m_name.c_str();
        event.m_element         = itr.second->m_name.c_str();
        event.m_clickCoords.m_x = MousePos.x;
        event.m_clickCoords.m_y = MousePos.y;

        const auto OriginalMousePos = getOriginalPoint(MousePos, m_guiManager);

        if (isInside(MousePos) && itr.second->isInside(OriginalMousePos) &&
            !m_titleBar.getGlobalBounds().contains(OriginalMousePos))
        {
            if (itr.second->getState() != GUI_ElementState::Neutral)
            {
                continue;
            }
            itr.second->onHover(MousePos);
            event.m_type = GUI_EventType::Hover;
            m_guiManager->addEvent(event);
        }
        else if (itr.second->getState() == GUI_ElementState::Focused)
        {
            itr.second->onLeave();
            event.m_type = GUI_EventType::Leave;
            m_guiManager->addEvent(event);
        }
    }
}

void GUI_Interface::draw(sf::RenderTarget* l_target)
{
    l_target->draw(m_backdrop);
    l_target->draw(m_content);
    l_target->draw(m_control);

    if (!m_showTitleBar)
    {
        return;
    }
    l_target->draw(m_titleBar);
    l_target->draw(m_visual.m_text);
}

auto GUI_Interface::isBeingMoved() const -> bool
{
    return m_beingMoved;
}
auto GUI_Interface::isMovable() const -> bool
{
    return m_movable;
}

void GUI_Interface::beginMoving()
{
    if (!m_showTitleBar || !m_movable)
    {
        return;
    }
    m_beingMoved    = true;
    m_moveMouseLast = m_guiManager->getContext()->m_window.getMousePosition();
}

void GUI_Interface::stopMoving()
{
    m_beingMoved = false;
}

auto GUI_Interface::getGlobalPosition() const -> sf::Vector2f
{
    sf::Vector2f   pos = m_position;
    GUI_Interface* i   = m_parent;
    while (i != nullptr)
    {
        pos += i->getPosition();
        i = i->m_parent;
    }
    return pos;
}

void GUI_Interface::applyStyle()
{
    GUI_Element::applyStyle(); // Call base method.
    m_visual.m_backgroundSolid.setPosition(0.F, 0.F);
    m_visual.m_backgroundImage.setPosition(0.F, 0.F);
    m_titleBar.setSize(sf::Vector2f(m_style[m_state].m_size.x, 16.F));
    m_titleBar.setPosition(m_position.x, m_position.y - m_titleBar.getSize().y);
    m_titleBar.setFillColor(m_style[m_state].m_elementColor);
    m_visual.m_text.setPosition(m_titleBar.getPosition() + m_style[m_state].m_textPadding);
    m_visual.m_glyph.setPosition(m_titleBar.getPosition() + m_style[m_state].m_glyphPadding);
}

void GUI_Interface::redraw()
{
    if (m_backdropTexture->getSize().x != m_style[m_state].m_size.x ||
        m_backdropTexture->getSize().y != m_style[m_state].m_size.y)
    {
        m_backdropTexture->create(m_style[m_state].m_size.x, m_style[m_state].m_size.y);
    }
    m_backdropTexture->clear(sf::Color(0, 0, 0, 0));
    applyStyle();
    m_backdropTexture->draw(m_visual.m_backgroundSolid);

    if (!m_style[m_state].m_backgroundImage.empty())
    {
        m_backdropTexture->draw(m_visual.m_backgroundImage);
    }

    m_backdropTexture->display();
    m_backdrop.setTexture(m_backdropTexture->getTexture());
    m_backdrop.setTextureRect(
        sf::IntRect(0, 0, m_style[m_state].m_size.x, m_style[m_state].m_size.y));
    setRedraw(false);
}
auto GUI_Interface::needsContentRedraw() const -> bool
{
    return m_contentRedraw;
}
void GUI_Interface::redrawContent()
{
    if (m_contentTexture->getSize().x != m_contentSize.x ||
        m_contentTexture->getSize().y != m_contentSize.y)
    {
        m_contentTexture->create(m_contentSize.x, m_contentSize.y);
    }

    m_contentTexture->clear(sf::Color(0, 0, 0, 0));

    for (auto& itr : m_elements)
    {
        GUI_Element* element = itr.second.get();
        if (!element->isActive() || element->isControl())
        {
            continue;
        }
        element->applyStyle();
        element->draw(m_contentTexture.get());
        element->setRedraw(false);
    }

    m_contentTexture->display();
    m_content.setTexture(m_contentTexture->getTexture());

    m_content.setTextureRect(sf::IntRect(m_scrollHorizontal, m_scrollVertical,
                                         m_style[m_state].m_size.x, m_style[m_state].m_size.y));
    m_contentRedraw = false;
}
auto GUI_Interface::needsControlRedraw() const -> bool
{
    return m_controlRedraw;
}
void GUI_Interface::redrawControls()
{
    if (m_controlTexture->getSize().x != m_style[m_state].m_size.x ||
        m_controlTexture->getSize().y != m_style[m_state].m_size.y)
    {
        m_controlTexture->create(m_style[m_state].m_size.x, m_style[m_state].m_size.y);
    }
    m_controlTexture->clear(sf::Color(0, 0, 0, 0));

    for (auto& itr : m_elements)
    {
        GUI_Element* element = itr.second.get();
        if (!element->isActive() || !element->isControl())
        {
            continue;
        }
        element->applyStyle();
        element->draw(m_controlTexture.get());
        element->setRedraw(false);
    }

    m_controlTexture->display();
    m_control.setTexture(m_controlTexture->getTexture());
    m_control.setTextureRect(
        sf::IntRect(0, 0, m_style[m_state].m_size.x, m_style[m_state].m_size.y));
    m_controlRedraw = false;
}

void GUI_Interface::toggleTitleBar()
{
    m_showTitleBar = !m_showTitleBar;
}

void GUI_Interface::adjustContentSize(const GUI_Element* l_reference)
{
    if (l_reference != nullptr)
    {
        sf::Vector2f bottom_right = l_reference->getPosition() + l_reference->getSize();
        if (bottom_right.x > m_contentSize.x)
        {
            m_contentSize.x = bottom_right.x;
            m_controlRedraw = true;
        }
        if (bottom_right.y > m_contentSize.y)
        {
            m_contentSize.y = bottom_right.y;
            m_controlRedraw = true;
        }
        return;
    }

    sf::Vector2f farthest = getSize();

    for (auto& itr : m_elements)
    {
        GUI_Element* element = itr.second.get();
        if (!element->isActive() || element->isControl())
        {
            continue;
        }
        sf::Vector2f bottom_right = element->getPosition() + element->getSize();
        if (bottom_right.x > farthest.x)
        {
            farthest.x      = bottom_right.x;
            m_controlRedraw = true;
        }
        if (bottom_right.y > farthest.y)
        {
            farthest.y      = bottom_right.y;
            m_controlRedraw = true;
        }
    }
    setContentSize(farthest);
}

void GUI_Interface::setContentSize(const sf::Vector2f& l_vec)
{
    m_contentSize = l_vec;
}

void GUI_Interface::updateScrollHorizontal(unsigned int l_percent)
{
    if (l_percent > 100)
    {
        return;
    }
    m_scrollHorizontal = ((m_contentSize.x - getSize().x) / 100) * l_percent;
    sf::IntRect rect   = m_content.getTextureRect();
    m_content.setTextureRect(
        sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
}

void GUI_Interface::updateScrollVertical(unsigned int l_percent)
{
    if (l_percent > 100)
    {
        return;
    }
    m_scrollVertical = ((m_contentSize.y - getSize().y) / 100) * l_percent;
    sf::IntRect rect = m_content.getTextureRect();
    m_content.setTextureRect(
        sf::IntRect(m_scrollHorizontal, m_scrollVertical, rect.width, rect.height));
}

auto GUI_Interface::getContentSize() const -> const sf::Vector2f&
{
    return m_contentSize;
}

void GUI_Interface::defocusTextfields()
{
    GUI_Event event{};
    event.m_type      = GUI_EventType::Release;
    event.m_interface = m_name.c_str();
    event.m_element   = "";
    for (auto& itr : m_elements)
    {
        if (itr.second->getType() != GUI_ElementType::Textfield)
        {
            continue;
        }
        itr.second->setState(GUI_ElementState::Neutral);
        event.m_element = itr.second->m_name.c_str();
        m_guiManager->addEvent(event);
    }
}