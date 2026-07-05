#ifndef CORE_ANIMATION_ANIMATION_HPP
#define CORE_ANIMATION_ANIMATION_HPP

#include <utils/utilities.hpp>
#include <core/animation/baseAnimation.hpp>

class Animation : public BaseAnimation
{
public:
    Animation() = default;
    ~Animation() override = default;

    void readInput(Utils::Tokens& l_tokens) override;

protected:
    void frameStep() override;
    void cropSprite() override;
};

#endif