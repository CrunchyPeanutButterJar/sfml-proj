#include "core/gui/GUI_element.hpp"
#include "core/gui/GUI_style.hpp"
#include <SFML/Window/Mouse.hpp>
#include <core/event_manager.hpp>
#include <core/gui/GUI_manager.hpp>
#include <core/window.hpp>
#include <ranges>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

using namespace core::gui;

static const std::unordered_map<std::string, GUI_ElementType> ELEMENT_TYPES = {
    {"Label", GUI_ElementType::Label},
    {"Button", GUI_ElementType::Button},
    {"Scrollbar", GUI_ElementType::Scrollbar},
    {"TextField", GUI_ElementType::Textfield},
    {"Interface", GUI_ElementType::Window}};

static auto stringToType(const std::string& l_string) -> GUI_ElementType
{
    auto it = ELEMENT_TYPES.find(l_string);
    return (it != ELEMENT_TYPES.end() ? it->second : GUI_ElementType::None);
}

GUI_Manager::GUI_Manager(EventManager* l_evMgr, SharedContext* l_shared)
    : m_eventMgr(l_evMgr), m_context(l_shared), m_currentState(core::state::StateType(0))
{
    registerElement<GUI_Label>(GUI_ElementType::Label);
    registerElement<GUI_Scrollbar>(GUI_ElementType::Scrollbar);
    registerElement<GUI_Textfield>(GUI_ElementType::Textfield);

    m_eventMgr->addCallback(core::state::StateType(0), "Mouse_ButtonPressedLeft",
                            [this](auto&& PH1) { handleClick(std::forward<decltype(PH1)>(PH1)); });
    m_eventMgr->addCallback(core::state::StateType(0), "Mouse_ButtonReleasedLeft",
                            [this](auto&& PH1)
                            { handleRelease(std::forward<decltype(PH1)>(PH1)); });
    m_eventMgr->addCallback(core::state::StateType(0), "Key_TextEntered", [this](auto&& PH1)
                            { handleTextEntered(std::forward<decltype(PH1)>(PH1)); });
}

GUI_Manager::~GUI_Manager()
{
    m_eventMgr->removeCallback(core::state::StateType(0), "Mouse_Left");
    m_eventMgr->removeCallback(core::state::StateType(0), "Mouse_Left_Release");
    m_eventMgr->removeCallback(core::state::StateType(0), "Text_Entered");
}

auto GUI_Manager::getInterface(state::StateType   l_state,
                               const std::string& l_name) -> GUI_Interface*
{
    auto s = m_interfaces.find(l_state);
    if (s == m_interfaces.end())
    {
        return nullptr;
    }
    auto i = s->second.find(l_name);
    return (i != s->second.end() ? i->second.get() : nullptr);
}

auto GUI_Manager::removeInterface(state::StateType l_state, const std::string& l_name) -> bool
{
    auto s = m_interfaces.find(l_state);
    if (s == m_interfaces.end())
    {
        return false;
    }
    auto i = s->second.find(l_name);
    if (i == s->second.end())
    {
        return false;
    }
    return s->second.erase(l_name) != 0U;
}
void GUI_Manager::setCurrentState(core::state::StateType l_state)
{
    if (m_currentState == l_state)
    {
        return;
    }
    handleRelease(core::EventDetails{});
    m_currentState = l_state;
}

auto GUI_Manager::getContext() -> core::SharedContext*
{
    return m_context;
}

void GUI_Manager::defocusAllInterfaces()
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end())
    {
        return;
    }
    for (auto& itr : state->second)
    {
        itr.second->defocus();
    }
}

void GUI_Manager::handleClick(const core::EventDetails& l_details)
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end())
    {
        return;
    }
    auto mouse_pos = l_details.m_newMousePos.value();
    for (auto& itr : std::ranges::reverse_view(state->second))
    {
        if (!itr.second->isInside(sf::Vector2f(mouse_pos)))
        {
            continue;
        }
        if (!itr.second->isActive())
        {
            continue;
        }
        itr.second->onClick(sf::Vector2f(mouse_pos));
        itr.second->focus();
        if (itr.second->isBeingMoved())
        {
            itr.second->beginMoving();
        }
        return;
    }
}

void GUI_Manager::handleRelease(const core::EventDetails& /*l_details*/)
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end())
    {
        return;
    }
    for (auto& itr : state->second)
    {
        auto* i = itr.second.get();
        if (!i->isActive())
        {
            continue;
        }
        if (i->getState() == GUI_ElementState::Clicked)
        {
            i->onRelease();
        }
        if (i->isBeingMoved())
        {
            i->stopMoving();
        }
    }
}

void GUI_Manager::handleTextEntered(const core::EventDetails& l_details)
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end())
    {
        return;
    }
    for (auto& itr : state->second)
    {
        if (!itr.second->isActive())
        {
            continue;
        }
        if (!itr.second->isFocused())
        {
            continue;
        }
        itr.second->onTextEntered(l_details.m_enteredText.value());
        return;
    }
}

void GUI_Manager::addEvent(GUI_Event l_event)
{
    m_events[m_currentState].push_back(l_event);
}

auto GUI_Manager::pollEvent(GUI_Event& l_event) -> bool
{
    if (m_events[m_currentState].empty())
    {
        return false;
    }
    l_event = m_events[m_currentState].back();
    m_events[m_currentState].pop_back();
    return true;
}
void GUI_Manager::update(float l_dT)
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end())
    {
        return;
    }
    for (auto& itr : std::ranges::reverse_view(state->second))
    {
        GUI_Interface* i = itr.second.get();
        if (!i->isActive())
        {
            continue;
        }
        i->update(l_dT);
        if (i->isBeingMoved())
        {
            continue;
        }

        const auto MousePos = sf::Mouse::getPosition(*getContext()->m_window.getRenderWindow());

        if (i->isInside(sf::Vector2f(MousePos)))
        {
            if (i->getState() == GUI_ElementState::Neutral)
            {
                i->onHover(sf::Vector2f(MousePos));
            }
            return;
        }
        if (i->getState() == GUI_ElementState::Focused)
        {
            i->onLeave();
        }
    }
}
void GUI_Manager::render(sf::RenderWindow* l_wind)
{
    auto state = m_interfaces.find(m_currentState);
    if (state == m_interfaces.end())
    {
        return;
    }
    for (auto& itr : state->second)
    {
        GUI_Interface* i = itr.second.get();
        if (!i->isActive())
        {
            continue;
        }
        if (i->needsRedraw())
        {
            i->redraw();
        }
        if (i->needsContentRedraw())
        {
            i->redrawContent();
        }
        if (i->needsControlRedraw())
        {
            i->redrawControls();
        }
        i->draw(l_wind);
    }
}
auto GUI_Manager::createElement(const GUI_ElementType& l_id,
                                GUI_Interface*         l_owner) -> GUI_ElementPtr
{
    if (l_id == GUI_ElementType::Window)
    {
        return std::make_unique<GUI_Interface>("", this);
    }
    auto f = m_factory.find(l_id);
    return (f != m_factory.end() ? f->second(l_owner) : nullptr);
}

auto GUI_Manager::addInterface(core::state::StateType l_state, const std::string& l_name) -> bool
{
    auto s = m_interfaces.emplace(l_state, GUI_Interfaces()).first;
    return s->second.emplace(l_name, std::make_unique<GUI_Interface>(l_name, this)).second;
}

auto GUI_Manager::loadInterface(core::state::StateType l_state, const std::string& l_interface,
                                const std::string& l_name) -> bool
{
    const std::string FilePath =
        utils::getResourcesDirectory() + "media/GUI_interfaces/" + l_interface;

    auto file = utils::readFile(FilePath);
    if (!file)
    {
        FAILURE_NON_FATAL("Failed to load interface file {}", FilePath);
        return false;
    }

    utils::Tokens              tokens{std::move(file.value())};
    std::optional<std::string> interface_name;

    while (!tokens.empty())
    {
        auto key = *consumeToken<std::string>(tokens);
        if (key == "Interface")
        {
            interface_name = std::string{};
            std::string style_file_name;

            std::tie(interface_name.value(), style_file_name) =
                *consumeTokens<std::string, std::string>(tokens);

            if (!addInterface(l_state, l_name))
            {
                FAILURE_NON_FATAL("Failed adding interface {}", l_name);
                return false;
            }

            auto* cur_interface = getInterface(l_state, l_name);
            cur_interface->readIn(tokens);

            if (!loadStyle(style_file_name, cur_interface))
            {
                FAILURE_NON_FATAL("Failed to load style file {} for interface {}", style_file_name,
                                  l_name);
            }
            cur_interface->setContentSize(cur_interface->getSize());
        }
        else if (key == "Element")
        {
            if (!interface_name.has_value())
            {
                FAILURE_NON_FATAL("'Element' outside or before declaration of 'Interface'");
                continue;
            }

            std::string  type;
            std::string  name;
            sf::Vector2f position;
            std::string  style;

            std::tie(type, name, position.x, position.y, style) =
                *consumeTokens<std::string, std::string, float, float, std::string>(tokens);
            auto e_type = stringToType(type);
            if (e_type == GUI_ElementType::None)
            {
                FAILURE_NON_FATAL("Unknow 'Element' {}  of type '{}'", name, type);
                continue;
            }

            if (auto* cur_interface = getInterface(l_state, l_name))
            {
                cur_interface->addElement(e_type, name);
                auto* element = cur_interface->getElement(name);
                element->readIn(tokens);
                element->setPosition(position);
                ASSERT_NON_FATAL(loadStyle(style, element),
                                 "Failed to load Style ({}) for Element ({})", style, name);
            }
        }
    }

    return true;
}

static auto stateToGuiState(const std::string& l_state) -> GUI_ElementState
{
    GUI_ElementState state{GUI_ElementState::Neutral};

    if (l_state == "Hover")
    {
        state = GUI_ElementState::Focused;
    }
    else if (l_state == "Clicked")
    {
        state = GUI_ElementState::Clicked;
    }

    return state;
}

auto GUI_Manager::loadStyle(const std::string& l_file, GUI_Element* l_element) -> bool
{
    const std::string FilePath = utils::getResourcesDirectory() + "media/GUI_styles/" + l_file;
    auto              file     = utils::readFile(FilePath);

    if (!file.has_value())
    {
        FAILURE_NON_FATAL("Failed to load {}", FilePath);
        return false;
    }

    utils::Tokens              tokens{std::move(file.value())};
    std::optional<std::string> current_state;
    GUI_Style                  parent_style;
    GUI_Style                  temporary_style;

    while (!tokens.empty())
    {
        const auto Key = *consumeToken<std::string>(tokens);
        if (Key == "State")
        {
            if (current_state.has_value())
            {
                FAILURE_NON_FATAL(
                    "Invalid Style Configuration in file {} : State inside another State {}",
                    FilePath, current_state.value());
                continue;
            }

            current_state = *consumeToken<std::string>(tokens);
        }
        else if (Key == "/State")
        {
            if (!current_state.has_value())
            {
                FAILURE_NON_FATAL("Invalid Style Configuration in file {} : no State before /State",
                                  FilePath);
                continue;
            }

            auto gui_state = stateToGuiState(*current_state);

            if (gui_state == GUI_ElementState::Neutral)
            {
                parent_style = temporary_style;
                l_element->updateStyle(GUI_ElementState::Neutral, temporary_style);
                l_element->updateStyle(GUI_ElementState::Focused, temporary_style);
                l_element->updateStyle(GUI_ElementState::Clicked, temporary_style);
            }
            else
            {
                l_element->updateStyle(gui_state, temporary_style);
            }

            temporary_style = parent_style;
            current_state.reset();
        }
        else
        {
            if (!current_state.has_value())
            {
                FAILURE_NON_FATAL(
                    "Invalid Style Configuration in file {} : Keyword ({}) outside of State",
                    FilePath, Key);
                continue;
            }

            if (Key == "Size")
            {
                std::tie(temporary_style.m_size.x, temporary_style.m_size.y) =
                    *consumeTokens<float, float>(tokens);
            }
            else if (Key == "BgColor")
            {
                int r                             = 0;
                int g                             = 0;
                int b                             = 0;
                int a                             = 0;
                std::tie(r, g, b, a)              = *consumeTokens<int, int, int, int>(tokens);
                temporary_style.m_backgroundColor = sf::Color(r, g, b, a);
            }
            else if (Key == "BgImage")
            {
                temporary_style.m_backgroundImage = *consumeToken<std::string>(tokens);
            }
            else if (Key == "BgImageColor")
            {
                int r                                  = 0;
                int g                                  = 0;
                int b                                  = 0;
                int a                                  = 0;
                std::tie(r, g, b, a)                   = *consumeTokens<int, int, int, int>(tokens);
                temporary_style.m_backgroundImageColor = sf::Color(r, g, b, a);
            }
            else if (Key == "TextColor")
            {
                int r                       = 0;
                int g                       = 0;
                int b                       = 0;
                int a                       = 0;
                std::tie(r, g, b, a)        = *consumeTokens<int, int, int, int>(tokens);
                temporary_style.m_textColor = sf::Color(r, g, b, a);
            }
            else if (Key == "TextSize")
            {
                temporary_style.m_textSize = *consumeToken<unsigned int>(tokens);
            }
            else if (Key == "TextOriginCenter")
            {
                temporary_style.m_textCenterOrigin = true;
            }
            else if (Key == "Font")
            {
                temporary_style.m_textFont = *consumeToken<std::string>(tokens);
            }
            else if (Key == "TextPadding")
            {
                std::tie(temporary_style.m_textPadding.x, temporary_style.m_textPadding.y) =
                    *consumeTokens<float, float>(tokens);
            }
            else if (Key == "ElementColor")
            {
                int r                          = 0;
                int g                          = 0;
                int b                          = 0;
                int a                          = 0;
                std::tie(r, g, b, a)           = *consumeTokens<int, int, int, int>(tokens);
                temporary_style.m_elementColor = sf::Color(r, g, b, a);
            }
            else if (Key == "Glyph")
            {
                temporary_style.m_glyph = *consumeToken<std::string>(tokens);
            }
            else if (Key == "GlyphPadding")
            {
                std::tie(temporary_style.m_glyphPadding.x, temporary_style.m_glyphPadding.y) =
                    *consumeTokens<float, float>(tokens);
            }
            else
            {
                FAILURE_NON_FATAL(
                    "Invalid Style Configuration in file {} : Invalid Style keyword ({})", FilePath,
                    Key);
            }
        }
    }

    return true;
}