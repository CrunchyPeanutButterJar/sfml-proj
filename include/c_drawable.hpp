#ifndef C_DRAWABLE_HPP
#define C_DRAWABLE_HPP

#include <c_base.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class C_Drawable : public C_Base
{
public:
    C_Drawable(Component l_type) : C_Base(l_type) {}
    virtual ~C_Drawable() = default;

    virtual void updatePosition(const sf::Vector2f& l_vec) = 0;
    virtual const sf::Vector2u& getSize() = 0;
    virtual void draw(sf::RenderWindow* l_window) = 0;
};

#endif