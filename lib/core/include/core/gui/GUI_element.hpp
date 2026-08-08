#ifndef CORE_GUI_GUI_ELEMENT_HPP
#define CORE_GUI_GUI_ELEMENT_HPP

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/font_manager.hpp>
#include <core/graphics/texture_manager.hpp>
#include <core/gui/GUI_style.hpp>
#include <string>
#include <unordered_map>
#include <utils/utilities.fwd.hpp>

#include <memory>

namespace core::gui
{

enum class GUI_ElementType
{
    None = -1,
    Window,
    Label,
    Button,
    Scrollbar,
    Textfield
};

struct StyleResources
{
    std::shared_ptr<sf::Texture> m_backgroundImage;
    std::shared_ptr<sf::Texture> m_glyph;
    std::shared_ptr<sf::Font>    m_font;
};

using ElementStyles          = std::unordered_map<GUI_ElementState, GUI_Style>;
using ElementStylesResources = std::unordered_map<GUI_ElementState, StyleResources>;

class GUI_Interface;

class GUI_Element
{
    friend class GUI_Interface;

  public:
    GUI_Element(std::string l_name, const GUI_ElementType& l_type, GUI_Interface* l_owner);
    virtual ~GUI_Element() = default;

    // Event methods.
    virtual void readIn(utils::Tokens& l_tokens)         = 0;
    virtual void onClick(const sf::Vector2f& l_mousePos) = 0;
    virtual void onRelease()                             = 0;
    virtual void onHover(const sf::Vector2f& l_mousePos) = 0;
    virtual void onLeave()                               = 0;
    virtual void update(float l_dT)                      = 0;
    virtual void draw(sf::RenderTarget* l_target)        = 0;

    // Non pure-virtual methods.
    virtual void updateStyle(const GUI_ElementState& l_state, const GUI_Style& l_style);
    virtual void applyStyle();

    auto getType() -> GUI_ElementType;

    [[nodiscard]] auto getName() const -> const std::string&;
    void               setName(const std::string& l_name);
    [[nodiscard]] auto getPosition() const -> const sf::Vector2f&;
    void               setPosition(const sf::Vector2f& l_pos);
    [[nodiscard]] auto getSize() const -> const sf::Vector2f&;
    [[nodiscard]] auto getState() const -> GUI_ElementState;
    void               setState(const GUI_ElementState& l_state);
    void               setRedraw(const bool& l_redraw);
    [[nodiscard]] auto needsRedraw() const -> bool;
    void               setOwner(GUI_Interface* l_owner);
    [[nodiscard]] auto getOwner() const -> GUI_Interface*;
    [[nodiscard]] auto hasOwner() const -> bool;
    [[nodiscard]] auto isActive() const -> bool;
    void               setActive(const bool& l_active);
    [[nodiscard]] auto isInside(const sf::Vector2f& l_point) const -> bool;
    [[nodiscard]] auto getGlobalPosition() const -> sf::Vector2f;
    [[nodiscard]] auto isControl() const -> bool;
    [[nodiscard]] auto getText() const -> std::string;
    void               setText(const std::string& l_text);

  protected:
    void applyTextStyle();
    void applyBgStyle();
    void applyGlyphStyle();

    std::string            m_name;
    sf::Vector2f           m_position;
    ElementStyles          m_style; // Style of drawables.
    ElementStylesResources m_stylesResources;
    GUI_Visual             m_visual; // Drawable bits.
    GUI_ElementType        m_type;
    GUI_ElementState       m_state{GUI_ElementState::Neutral};
    GUI_Interface*         m_owner;

    bool m_needsRedraw{false};
    bool m_active{true};
    bool m_isControl{false};
};

using GUI_ElementPtr = std::unique_ptr<GUI_Element>;

} // namespace core::gui

#endif