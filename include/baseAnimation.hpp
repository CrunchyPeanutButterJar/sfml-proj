#ifndef BASEANIMATION_HPP
#define BASEANIMATION_HPP

#include <baseAnimation.fwd.hpp>
#include <spriteSheet.fwd.hpp>

#include <vector>

using Frame = unsigned int;

class BaseAnimation
{
friend class SpriteSheet;

public:
    BaseAnimation() = default;
    virtual ~BaseAnimation() = default;

    void play();
    void pause();
    void stop();
    void reset();

    virtual void update(float l_dt);
    virtual void readInput(std::vector<std::string> &l_tokens) = 0;

    void setFrame(Frame l_frame);
    Frame getFrame() const;

    bool nextFrame();

    bool isInAction() const;

  protected:
    virtual void frameStep() = 0;
    virtual void cropSprite() = 0;

protected:
  Frame m_frameCurrent{0};
  Frame m_frameStart{0};
  Frame m_frameEnd{0};
  Frame m_frameRow{0};
  int m_frameActionStart{-1};
  int m_frameActionEnd{-1};
  float m_frameTime{0.f};
  float m_elapsedTime{0.f};
  bool m_loop{false};
  bool m_playing{false};
  std::string m_name;
  SpriteSheet *m_spriteSheet;
};

#endif