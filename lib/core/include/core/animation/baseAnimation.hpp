#ifndef CORE_ANIMATION_BASEANIMATION_HPP
#define CORE_ANIMATION_BASEANIMATION_HPP

#include <SFML/Graphics/Texture.hpp>
#include <utils/utilities.hpp>
#include <core/graphics/spriteSheet.fwd.hpp>

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
    void loop();
    void stopLoop();

    virtual void update(float l_dt);
    virtual void readInput(Utils::Tokens& l_tokens) = 0;

    Frame getFrame() const;

    bool nextFrame();

    bool isInAction() const;

  protected:
    virtual void frameStep() = 0;
    virtual void cropSprite() = 0;


  Frame m_frameCurrent{0};
  Frame m_frameStart{0};
  Frame m_frameEnd{0};
  Frame m_frameRow{0};
  int m_frameActionStart{-1};
  int m_frameActionEnd{-1};
  float m_frameTime{0.F};
  float m_elapsedTime{0.F};
  bool m_loop{false};
  bool m_playing{false};
  std::string m_name;
  SpriteSheet *m_spriteSheet{};
  sf::Texture* m_texture{};
};

#endif