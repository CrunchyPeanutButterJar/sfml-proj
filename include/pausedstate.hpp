#ifndef PAUSEDSTATE_HPP
#define PAUSEDSTATE_HPP

#include <basestate.hpp>

class PausedState : public BaseState
{
public:
    PausedState(StateManager& l_stateManager);
    ~PausedState() override;

    void Update(const sf::Time& l_elapsed) override {};
    void Draw() override;

    void Activate() override {};
    void Deactivate() override {};
};

#endif