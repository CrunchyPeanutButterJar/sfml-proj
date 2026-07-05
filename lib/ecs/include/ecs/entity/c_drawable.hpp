#ifndef ECS_ENTITY_C_DRAWABLE_HPP
#define ECS_ENTITY_C_DRAWABLE_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <ecs/entity/c_base.hpp>

namespace ecs::entity
{
class CDrawable : public CBase
{
  public:
    CDrawable(Component l_type) : CBase(l_type) {}
    ~CDrawable() override = default;

    virtual void updatePosition(const sf::Vector2f& l_vec) = 0;
    virtual auto getSize() -> const sf::Vector2u&          = 0;
    virtual void draw(sf::RenderWindow* l_window)          = 0;
};
} // namespace ecs::entity

#endif