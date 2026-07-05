#ifndef ECS_ENTITY_C_DRAWABLE_HPP
#define ECS_ENTITY_C_DRAWABLE_HPP

#include <ecs/entity/c_base.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

class CDrawable : public CBase
{
public:
    CDrawable(Component l_type) : CBase(l_type) {}
    ~CDrawable() override = default;

    virtual void updatePosition(const sf::Vector2f& l_vec) = 0;
    virtual const sf::Vector2u& getSize() = 0;
    virtual void draw(sf::RenderWindow* l_window) = 0;
};

#endif