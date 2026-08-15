#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <core/graphics/gif.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

namespace core::graphics
{

static auto getNumberOfImagesPerRow(const sf::Vector2u& l_imageSize,
                                    const sf::Vector2u& l_textureSize) -> std::uint8_t
{
    // row width / image width
    return l_textureSize.x / l_imageSize.x;
}

static auto getNumberOfRows(const sf::Vector2u& l_imageSize,
                            const sf::Vector2u& l_textureSize) -> std::uint8_t
{
    // col height / image height
    return l_textureSize.y / l_imageSize.y;
}

GifInfo::GifInfo(const sf::Vector2u& l_imageSize, const sf::Vector2u& l_textureSize)
    : m_imageSize{l_imageSize},
      m_numberOfImagesPerRow(getNumberOfImagesPerRow(l_imageSize, l_textureSize)),
      m_numberOfRows(getNumberOfRows(l_imageSize, l_textureSize))
{
}

Gif::Gif(core::graphics::TextureManager& l_textureManager) : m_textureManager{l_textureManager} {}

auto Gif::loadGif(const std::string& l_filePath) -> bool
{
    const std::string Path = utils::getResourcesDirectory() + l_filePath;
    auto              file = utils::readFile(Path);
    if (!file)
    {
        FAILURE_NON_FATAL("Failed to load gif file : {}", Path);
        return false;
    }

    utils::Tokens tokens{std::move(*file)};

    std::optional<sf::Vector2u> image_size;

    while (!tokens.empty())
    {
        auto key = *consumeToken<std::string>(tokens);

        if (key == "Size")
        {
            sf::Vector2u temp;
            std::tie(temp.x, temp.y) = *consumeTokens<unsigned int, unsigned int>(tokens);
            image_size               = temp;
        }
        else if (key == "Texture")
        {
            m_texture = m_textureManager.acquire(*consumeToken<std::string>(tokens));
        }
        else if (key == "FrameTime")
        {
            m_frameTime = *consumeToken<float>(tokens);
        }
        else if (key == "Loop")
        {
            m_loop = true;
        }
        else
        {
            FAILURE_NON_FATAL("Invalid Key {} in gif file {}", key, Path);
            return false;
        }
    }

    if (!image_size.has_value() || nullptr == m_texture)
    {
        FAILURE_NON_FATAL("Gif file {} is missing information", Path);
        return false;
    }

    m_gifInfo = GifInfo{image_size.value(), m_texture->getSize()};
    m_image.setTexture(*m_texture);
    cropSprite();

    return true;
}

void Gif::frameStep() // reading from left to right, top to bottom
{
    const std::uint8_t MaxId = m_gifInfo.m_numberOfImagesPerRow * m_gifInfo.m_numberOfRows;

    m_currentImageIndex = (m_currentImageIndex + 1) % MaxId;

    if (m_currentImageIndex == 0 && !m_loop)
    {
        m_playing = false;
    }
}

void Gif::cropSprite()
{
    std::uint8_t i_row = m_currentImageIndex / m_gifInfo.m_numberOfImagesPerRow;
    std::uint8_t i_col = m_currentImageIndex % m_gifInfo.m_numberOfImagesPerRow;

    sf::IntRect rect{static_cast<int>(i_col * m_gifInfo.m_imageSize.x),
                     static_cast<int>(i_row * m_gifInfo.m_imageSize.y),
                     static_cast<int>(m_gifInfo.m_imageSize.x),
                     static_cast<int>(m_gifInfo.m_imageSize.y)};

    m_image.setTextureRect(rect);
}

auto Gif::getImageSize() const -> const sf::Vector2u&
{
    return m_gifInfo.m_imageSize;
}

void Gif::scale(float l_scaleWidth, float l_scaleHeight)
{
    m_image.setScale({l_scaleWidth, l_scaleHeight});
}

void Gif::update(float l_dt)
{
    if (!m_playing)
    {
        return;
    }

    m_timeElapsed += l_dt;
    if (m_timeElapsed < m_frameTime)
    {
        return;
    }

    m_timeElapsed -= m_frameTime;
    frameStep();
    cropSprite();
}

void Gif::draw(sf::RenderWindow* l_window)
{
    l_window->draw(m_image);
}

auto Gif::isDone() const -> bool
{
    return m_playing;
}

void Gif::reset()
{
    m_playing           = true;
    m_currentImageIndex = 0;
    cropSprite();
}
} // namespace core::graphics