#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <baseAnimation.hpp>

class Animation : public BaseAnimation
{
public:
    Animation() = default;
    ~Animation() = default;

    void readInput(std::vector<std::string> &tokens) override;

protected:
    void frameStep() override;
    void cropSprite() override;
};

#endif