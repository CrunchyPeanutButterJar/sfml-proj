#ifndef CORE_GUI_GUI_INTERFACE_HPP
#define CORE_GUI_GUI_INTERFACE_HPP

#include <utils/utilities.fwd.hpp>
#include <core/gui/GUI_element.hpp>
#include <unordered_map>

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace core::gui
{
using Elements = std::unordered_map<std::string, GUI_ElementPtr>;

class GUI_Manager;

class GUI_Interface : public GUI_Element
{
    friend class GUI_Element;
    friend class GUI_Manager;

  public:
    GUI_Interface(const std::string& l_name, GUI_Manager* l_guiManager);
    ~GUI_Interface() override;

    void setPosition(const sf::Vector2f& l_pos);

    auto               addElement(const GUI_ElementType& l_type, const std::string& l_name) -> bool;
    [[nodiscard]] auto getElement(const std::string& l_name) const -> GUI_Element*;
    auto               removeElement(const std::string& l_name) -> bool;

    [[nodiscard]] auto hasParent() const -> bool;
    [[nodiscard]] auto getManager() const -> GUI_Manager*;

    [[nodiscard]] auto isInside(const sf::Vector2f& l_point) const -> bool;

    void focus();
    void defocus();
    auto isFocused() const -> bool;

    void readIn(utils::Tokens& l_tokens) override;
    void onClick(const sf::Vector2f& l_mousePos) override;
    void onRelease() override;
    void onHover(const sf::Vector2f& l_mousePos) override;
    void onLeave() override;
    void onTextEntered(const char& l_char);

    [[nodiscard]] auto isBeingMoved() const -> bool;
    [[nodiscard]] auto isMovable() const -> bool;
    void               beginMoving();
    void               stopMoving();

    void applyStyle() override;

    [[nodiscard]] auto getPadding() const -> const sf::Vector2f&;
    void               setPadding(const sf::Vector2f& l_padding);
    [[nodiscard]] auto getGlobalPosition() const -> sf::Vector2f;
    [[nodiscard]] auto getContentSize() const -> const sf::Vector2f&;

    void               redraw();
    [[nodiscard]] auto needsContentRedraw() const -> bool;
    void               redrawContent();
    [[nodiscard]] auto needsControlRedraw() const -> bool;
    void               redrawControls();

    void toggleTitleBar();

    void update(float l_dT) override;
    void draw(sf::RenderTarget* l_target) override;

    void updateScrollHorizontal(unsigned int l_percent);
    void updateScrollVertical(unsigned int l_percent);

  private:
    void         defocusTextfields();
    Elements     m_elements;
    sf::Vector2f m_elementPadding;

    GUI_Interface* m_parent{nullptr};
    GUI_Manager*   m_guiManager;

    std::unique_ptr<sf::RenderTexture> m_backdropTexture;
    sf::Sprite                         m_backdrop;

    // Movement.
    sf::RectangleShape m_titleBar;
    sf::Vector2f       m_moveMouseLast;
    bool               m_showTitleBar{false};
    bool               m_movable{false};
    bool               m_beingMoved{false};
    bool               m_focused{false};

    // Variable size.
    void                               adjustContentSize(const GUI_Element* l_reference = nullptr);
    void                               setContentSize(const sf::Vector2f& l_vec);
    std::unique_ptr<sf::RenderTexture> m_contentTexture;
    sf::Sprite                         m_content;
    sf::Vector2f                       m_contentSize;
    int                                m_scrollHorizontal{0};
    int                                m_scrollVertical{0};
    bool                               m_contentRedraw{true};

    // Control layer.
    std::unique_ptr<sf::RenderTexture> m_controlTexture;
    sf::Sprite                         m_control;
    bool                               m_controlRedraw{true};
};

using GUI_InterfacePtr = std::unique_ptr<GUI_Interface>;
} // namespace core::gui
#endif
