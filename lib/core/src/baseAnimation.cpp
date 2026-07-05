#include <core/animation/baseAnimation.hpp>

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
    m_elapsedTime = 0.0F;
    cropSprite();   
}

void BaseAnimation::update(float l_dt)
{
    if(!m_playing)
    {
        return;
    }

    m_elapsedTime += l_dt;
    if(m_elapsedTime < m_frameTime)
    {
        return;
    }
    frameStep();
    cropSprite();
    m_elapsedTime-=m_frameTime;
}

Frame BaseAnimation::getFrame() const
{
    return m_frameCurrent;
}

bool BaseAnimation::nextFrame()
{
    if(m_frameStart < m_frameEnd)
    {
        if(m_frameCurrent >= m_frameEnd)
        {
            return false;
        }
        m_frameCurrent++;
    }
    else
    {
        if(m_frameCurrent <= m_frameEnd)
        {
            return false;
        }

        m_frameCurrent--;
    }

    return true;
}

bool BaseAnimation::isInAction() const
{
    if(m_frameActionStart == -1 || m_frameActionEnd == -1)
    {
        return true;
    }

    return  (((int)m_frameCurrent >= m_frameActionStart) && ((int)m_frameCurrent <= m_frameActionEnd));
}