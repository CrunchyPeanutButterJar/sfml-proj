#ifndef CORE_STATE_STATEMANAGER_HPP
#define CORE_STATE_STATEMANAGER_HPP

#include <core/graphics/texture_manager.hpp>
#include <core/shared_context.hpp>
#include <core/state/basestate.hpp>
#include <core/state/statemanager.fwd.hpp>

#include <SFML/System/Time.hpp>

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace core::state
{

using StatePtr       = std::unique_ptr<BaseState>;
using StateContainer = std::vector<std::pair<StateType, StatePtr>>;

using StateFactory = std::unordered_map<StateType, std::function<StatePtr(StateManager*)>>;

using StateTypeContainer = std::unordered_set<StateType>;

class StateManager
{
  private:
    StateManager(SharedContext& l_sharedContext) : m_context{l_sharedContext} {}

  public:
    void update(const sf::Time& l_elapsed);
    void draw();

    void switchTo(StateType l_state);
    void remove(StateType l_state);
    void processRequests();

    [[nodiscard]] auto getCurrentState() const -> StateType;
    [[nodiscard]] auto hasState(StateType l_state) const -> bool;

    auto getContext() -> SharedContext*;

    template <typename T> auto getContext() -> T* { return static_cast<T*>(&m_context); }

    template <typename... States> static auto build(SharedContext& l_sharedContext) -> StateManager
    {
        StateManager state_manager{l_sharedContext};
        (state_manager.registerState<States>(States::TYPE), ...);
        return state_manager;
    }

  private:
    template <typename StateImpl> void registerState(StateType l_stateType)
    {
        m_stateFactory[l_stateType] = [](StateManager* stateManager) -> StatePtr
        {
            auto state = std::make_unique<StateImpl>(*stateManager);
            state->onCreate();

            return state;
        };
    }

    SharedContext&     m_context;
    StateContainer     m_states;
    StateFactory       m_stateFactory;
    StateTypeContainer m_toBeRemoved;
};
} // namespace core::state

#endif