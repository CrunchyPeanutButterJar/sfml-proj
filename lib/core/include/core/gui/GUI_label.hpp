#ifndef CORE_GUI_GUI_LABEL_HPP
#define CORE_GUI_GUI_LABEL_HPP

#include <core/gui/GUI_element.hpp>
#include <utils/utilities.fwd.hpp>

namespace core::gui
{
class GUI_Label : public GUI_Element
{
  public:
    GUI_Label(const std::string& l_name, GUI_Interface* l_owner);
    ~GUI_Label() override;

    void readIn(utils::Tokens& l_tokens) override;
    void onClick(const sf::Vector2f& l_mousePos) override;
    void onRelease() override;
    void onHover(const sf::Vector2f& l_mousePos) override;
    void onLeave() override;
    void update(float l_dT) override;
    void draw(sf::RenderTarget* l_target) override;
};
} // namespace core::gui

#endif