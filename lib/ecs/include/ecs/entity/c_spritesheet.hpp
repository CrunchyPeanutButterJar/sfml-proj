#ifndef ECS_ENTITY_C_SPRITESHEET_HPP
#define ECS_ENTITY_C_SPRITESHEET_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/graphics/spritesheet.hpp>
#include <core/graphics/texture_manager.fwd.hpp>
#include <ecs/entity/c_drawable.hpp>
#include <optional>
#include <utils/utilities.fwd.hpp>

#include <string>

namespace ecs::entity
{
class CSpriteSheet : public CDrawable
{
  public:
    CSpriteSheet();
    void readInput(utils::Tokens& l_tokens) override;
    void create(core::graphics::TextureManager& l_textureManager,
                std::optional<std::string>      l_sheetName = {});

    auto getSpriteSheet() -> core::graphics::SpriteSheet*;

    void updatePosition(const sf::Vector2f& l_vec) override;
    auto getSize() -> const sf::Vector2u& override;
    void draw(sf::RenderWindow* l_window) override;

    static bool debug_overlay;

  private:
    std::optional<core::graphics::SpriteSheet> m_spriteSheet;
    std::string                                m_sheetName;
};
} // namespace ecs::entity

#endif