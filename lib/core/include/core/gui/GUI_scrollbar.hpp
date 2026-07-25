#ifndef CORE_GUI_GUI_SCROLLBAR_HPP
#define CORE_GUI_GUI_SCROLLBAR_HPP

#include <core/gui/GUI_element.hpp>

namespace core::gui
{

enum class SliderType
{
    Horizontal,
    Vertical
};

class GUI_Scrollbar : public GUI_Element
{
  public:
    GUI_Scrollbar(const std::string& l_name, GUI_Interface* l_owner);
    ~GUI_Scrollbar() override;

    void setPosition(const sf::Vector2f& l_pos);

    void readIn(utils::Tokens& l_tokens) override;
    void onClick(const sf::Vector2f& l_mousePos) override;
    void onRelease() override;
    void onHover(const sf::Vector2f& l_mousePos) override;
    void onLeave() override;

    void applyStyle() override;
    void updateStyle(const GUI_ElementState& l_state, const GUI_Style& l_style) override;

    void update(float l_dT) override;
    void draw(sf::RenderTarget* l_target) override;

  private:
    SliderType         m_sliderType;
    sf::RectangleShape m_slider;
    sf::Vector2f       m_moveMouseLast;
};
} // namespace core::gui
#endif
