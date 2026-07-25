#ifndef CORE_GUI_GUI_TEXTFIELD_HPP
#define CORE_GUI_GUI_TEXTFIELD_HPP

#include <core/gui/GUI_element.hpp>
#include <utils/utilities.hpp>

class GUI_Textfield : public core::gui::GUI_Element
{
  public:
    GUI_Textfield(const std::string& l_name, core::gui::GUI_Interface* l_owner);
    ~GUI_Textfield() override;

    void readIn(utils::Tokens& l_tokens) override;
    void onClick(const sf::Vector2f& l_mousePos) override;
    void onRelease() override;
    void onHover(const sf::Vector2f& l_mousePos) override;
    void onLeave() override;
    void update(float l_dT) override;
    void draw(sf::RenderTarget* l_target) override;
};
#endif