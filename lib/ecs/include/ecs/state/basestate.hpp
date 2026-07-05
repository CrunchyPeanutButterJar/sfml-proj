#ifndef ECS_STATE_BASESTATE_HPP
#define ECS_STATE_BASESTATE_HPP

#include <SFML/Graphics/View.hpp>
#include <SFML/System/Time.hpp>

namespace ecs::state
{
class StateManager;

class BaseState
{
  public:
    BaseState(StateManager& l_stateManager);

    virtual ~BaseState() = default;

    virtual void update(const sf::Time& l_elapsed) = 0;
    virtual void draw()                            = 0;

    virtual void activate()   = 0;
    virtual void deactivate() = 0;

    virtual void onCreate() = 0;

    void setTransparent(bool l_transparent) { m_transparent = l_transparent; }
    auto isTransparent() const -> bool { return m_transparent; }

    void setTranscendent(bool l_transcendent) { m_transcendent = l_transcendent; }
    auto isTranscendent() const -> bool { return m_transcendent; }

    auto getView() const -> sf::View { return m_view; }

  protected:
    StateManager& m_stateManager;
    sf::View      m_view;

  private:
    bool m_transparent{false};
    bool m_transcendent{false};

    friend class StateManager;
};
} // namespace ecs::state

#endif