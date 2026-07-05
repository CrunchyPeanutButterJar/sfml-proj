#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP

#include <sharedContext.hpp>
#include <basestate.hpp>
#include <core/graphics/texture_manager.hpp>

#include <SFML/System/Time.hpp>

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>


enum class StateType : std::uint8_t
{
    Game = 1,
};


using StatePtr = std::unique_ptr<BaseState>;
using StateContainer = std::vector<std::pair<StateType, StatePtr>>;

using StateFactory = std::unordered_map<StateType, std::function<StatePtr(StateManager*)>>;

using StateTypeContainer = std::unordered_set<StateType>;

class StateManager
{
public:
    StateManager(SharedContext l_sharedContext);

    void update(const sf::Time& l_elapsed);
    void draw();

    void switchTo(StateType l_state);
    void remove(StateType l_state);
    void processRequests();

    [[nodiscard]] auto getCurrentState() const -> StateType;
    [[nodiscard]] auto hasState(StateType l_state) const -> bool;

    auto getContext() -> SharedContext&;

private:
    template<typename StateImpl> void registerState(StateType l_stateType);


    SharedContext m_context;
    StateContainer m_states;
    StateFactory m_stateFactory;
    StateTypeContainer m_toBeRemoved;
};

#endif