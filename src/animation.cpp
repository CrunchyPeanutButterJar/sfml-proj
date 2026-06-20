#include <animation.hpp>
#include <utilities/utilities.hpp>
#include <spriteSheet.hpp>
#include <SFML/Graphics/Rect.hpp>

void Animation::cropSprite()
{
    auto [width, height] = m_spriteSheet->getSpriteSize();

    sf::IntRect rect
    {
        width * (int)getFrame(),
        height * (int)m_frameRow,
        width,
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
    std::tie
    (
        m_frameStart,
        m_frameEnd,
        m_frameRow,
        m_frameTime,
        m_frameActionStart,
        m_frameActionEnd
    ) = Utils::ConsumeTokens<Frame, Frame, Frame, float, int, int>(l_tokens);
}