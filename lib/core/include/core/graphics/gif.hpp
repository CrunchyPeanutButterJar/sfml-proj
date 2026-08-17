#ifndef CORE_GRAPHICS_GIF_HPP
#define CORE_GRAPHICS_GIF_HPP

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/graphics/gif.fwd.hpp>
#include <core/graphics/texture_manager.hpp>

#include <memory>

namespace core::graphics
{

struct GifInfo
{
    GifInfo() = default;
    GifInfo(const sf::Vector2u& l_imageSize, const sf::Vector2u& l_textureSize);

    sf::Vector2u m_imageSize; /*size of individual image in texture*/
    std::uint8_t m_numberOfImagesPerRow{};
    std::uint8_t m_numberOfRows{};
};

class Gif
{
  public:
    Gif(core::graphics::TextureManager& l_textureManager);

    auto loadGif(const std::string& l_filePath) -> bool;

    void update(float l_dt);
    void draw(sf::RenderWindow* l_window);
    void scale(float l_scaleWidth, float l_scaleHeight);
    void setPosition(const sf::Vector2f& l_newPos);

    [[nodiscard]] auto isDone() const -> bool;
    [[nodiscard]] auto getImageSize() const -> const sf::Vector2u&;

    void reset();

  private:
    void frameStep();
    void cropSprite();

    float                           m_timeElapsed{0.F};
    float                           m_frameTime{1.F};
    bool                            m_loop{false};
    bool                            m_playing{true};
    GifInfo                         m_gifInfo;
    std::uint8_t                    m_currentImageIndex{};
    core::graphics::TextureManager& m_textureManager;
    sf::Sprite                      m_image;
    std::shared_ptr<sf::Texture>    m_texture;
};

} // namespace core::graphics

#endif