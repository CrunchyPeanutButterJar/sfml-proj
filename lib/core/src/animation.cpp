#include <core/animation/animation.hpp>
#include <utils/utilities.hpp>
#include <core/graphics/spriteSheet.hpp>
#include <SFML/Graphics/Rect.hpp>

using namespace core::animation;

void Animation::cropSprite()
{
    auto [uwidth, uheight] = m_spriteSheet->getSpriteSize();
    int width = (int) uwidth;
    int height = (int) uheight;

    Direction current_direction = m_spriteSheet->getDirection();

    sf::IntRect rect
    {
        width * (current_direction == Direction::Right? (int)getFrame() : (int) getFrame() + 1),
        height * (int)m_frameRow,
        current_direction == Direction::Right? width : -width,
        height
    };

    m_spriteSheet->cropSprite(rect);
}

void Animation::frameStep()
{
    const bool Advanced = nextFrame();
    if(!Advanced)
    {
        if(m_loop)
        {
            m_frameCurrent = m_frameStart;
            return;
        }
        pause();
    }
}

void Animation::readInput(utils::Tokens& l_tokens)
{
    auto tuple = utils::consumeTokens<Frame, Frame, Frame, float, int, int>(l_tokens);

    ASSERT(tuple.has_value(), "Error reading Animation from config file");

    std::tie
    (
        m_frameStart,
        m_frameEnd,
        m_frameRow,
        m_frameTime,
        m_frameActionStart,
        m_frameActionEnd
    ) = *tuple;
}