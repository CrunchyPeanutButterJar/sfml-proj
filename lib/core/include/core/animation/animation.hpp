#ifndef CORE_ANIMATION_ANIMATION_HPP
#define CORE_ANIMATION_ANIMATION_HPP

#include <core/animation/baseAnimation.hpp>
#include <utils/utilities.hpp>

namespace core::animation
{
class Animation : public BaseAnimation
{
  public:
    Animation()           = default;
    ~Animation() override = default;

    void readInput(utils::Tokens& l_tokens) override;

  protected:
    void frameStep() override;
    void cropSprite() override;
};

} // namespace core::animation

#endif