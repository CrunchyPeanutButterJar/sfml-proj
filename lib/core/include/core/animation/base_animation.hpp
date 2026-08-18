#ifndef CORE_ANIMATION_BASE_ANIMATION_HPP
#define CORE_ANIMATION_BASE_ANIMATION_HPP

#include <SFML/Graphics/Texture.hpp>
#include <core/graphics/spritesheet.fwd.hpp>
#include <utils/utilities.fwd.hpp>

namespace core::animation
{

using Frame = unsigned int;
class BaseAnimation
{
    friend class core::graphics::SpriteSheet;

  public:
    BaseAnimation()          = default;
    virtual ~BaseAnimation() = default;

    void play();
    void pause();
    void stop();
    void reset();
    void loop();
    void stopLoop();

    virtual void update(float l_dt);
    virtual void readInput(utils::Tokens& l_tokens) = 0;

    [[nodiscard]] auto getFrame() const -> Frame;

    auto nextFrame() -> bool;

    [[nodiscard]] auto isInAction() const -> bool;
    [[nodiscard]] auto hasMoved() const -> bool;
    [[nodiscard]] auto isDone() const -> bool;

  protected:
    virtual void frameStep()  = 0;
    virtual void cropSprite() = 0;

    Frame                        m_frameCurrent{0};
    Frame                        m_frameStart{0};
    Frame                        m_frameEnd{0};
    Frame                        m_frameRow{0};
    int                          m_frameActionStart{-1};
    int                          m_frameActionEnd{-1};
    float                        m_frameTime{0.F};
    float                        m_elapsedTime{0.F};
    bool                         m_loop{false};
    bool                         m_playing{false};
    std::string                  m_name;
    core::graphics::SpriteSheet* m_spriteSheet{};
    sf::Texture*                 m_texture{};
    mutable bool                 m_hasMoved{false};
};
} // namespace core::animation

#endif