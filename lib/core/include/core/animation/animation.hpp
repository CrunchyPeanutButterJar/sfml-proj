#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <utils/utilities.hpp>
#include <core/animation/baseAnimation.hpp>

class Animation : public BaseAnimation
{
public:
    Animation() = default;
    ~Animation() = default;

    void readInput(Utils::Tokens& l_tokens) override;

protected:
    void frameStep() override;
    void cropSprite() override;
};

#endif