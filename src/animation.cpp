#include <animation.hpp>
#include <utils/utilities.hpp>
#include <spriteSheet.hpp>
#include <SFML/Graphics/Rect.hpp>

void Animation::cropSprite()
{
    auto [width, height] = m_spriteSheet->getSpriteSize();

    Direction currentDirection = m_spriteSheet->getDirection();

    sf::IntRect rect
    {
        width * (currentDirection == Direction::Right? (int)getFrame() : (int) getFrame() + 1),
        height * (int)m_frameRow,
        currentDirection == Direction::Right? width : -width,
        height
    };

    m_spriteSheet->cropSprite(rect);
}

void Animation::frameStep()
{
    const bool advanced = nextFrame();
    if(!advanced)
    {
        if(m_loop)
        {
            m_frameCurrent = m_frameStart;
            return;
        }
        pause();
    }
}

void Animation::readInput(Utils::Tokens& l_tokens)
{
    auto tuple = Utils::ConsumeTokens<Frame, Frame, Frame, float, int, int>(l_tokens);

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