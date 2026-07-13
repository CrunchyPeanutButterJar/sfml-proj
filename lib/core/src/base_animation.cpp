#include <core/animation/base_animation.hpp>

using namespace core::animation;

void BaseAnimation::play()
{
    m_playing = true;
}

void BaseAnimation::loop()
{
    m_loop = true;
}

void BaseAnimation::stopLoop()
{
    m_loop = false;
}

void BaseAnimation::pause()
{
    m_playing = false;
}

void BaseAnimation::stop()
{
    m_playing = false;
    reset();
}

void BaseAnimation::reset()
{
    m_frameCurrent = m_frameStart;
    m_elapsedTime  = 0.0F;
    cropSprite();
}

void BaseAnimation::update(float l_dt)
{
    if (!m_playing)
    {
        return;
    }

    m_elapsedTime += l_dt;
    if (m_elapsedTime < m_frameTime)
    {
        return;
    }
    frameStep();
    cropSprite();
    m_elapsedTime -= m_frameTime;
}

auto BaseAnimation::getFrame() const -> Frame
{
    return m_frameCurrent;
}

auto BaseAnimation::nextFrame() -> bool
{
    if (m_frameStart < m_frameEnd)
    {
        if (m_frameCurrent >= m_frameEnd)
        {
            return false;
        }
        m_frameCurrent++;
    }
    else
    {
        if (m_frameCurrent <= m_frameStart)
        {
            return false;
        }

        m_frameCurrent--;
    }

    return true;
}

auto BaseAnimation::isInAction() const -> bool
{
    if (m_frameActionStart == -1 || m_frameActionEnd == -1)
    {
        return true;
    }

    return (((int)m_frameCurrent >= m_frameActionStart) &&
            ((int)m_frameCurrent <= m_frameActionEnd));
}