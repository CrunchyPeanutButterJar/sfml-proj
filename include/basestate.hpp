#ifndef BASESTATE_HPP
#define BASESTATE_HPP

#include <SFML/System/Time.hpp>

class StateManager;

class BaseState
{
public:
    inline BaseState(StateManager& l_stateManager) : m_stateManager(l_stateManager) {}
    virtual ~BaseState() = default;

    virtual void Update(const sf::Time& l_elapsed) = 0;
    virtual void Draw() = 0;

    virtual void Activate() = 0;
    virtual void Deactivate() = 0;

    inline void SetTransparent(bool l_transparent) { m_transparent = l_transparent; }
    inline bool IsTransparent() const { return m_transparent; }

    inline void SetTranscendent(bool l_transcendent) { m_transcendent = l_transcendent; }
    inline bool IsTranscendent() const { return m_transcendent; }

protected:
    StateManager& m_stateManager;

private:
    bool m_transparent{false};
    bool m_transcendent{false};
};

#endif