#ifndef STATEMANAGER_HPP
#define STATEMANAGER_HPP

#include <sharedContext.hpp>
#include <basestate.hpp>
#include <textureManager.hpp>

#include <SFML/System/Time.hpp>

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>


enum class StateType
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

    void Update(const sf::Time& l_elapsed);
    void Draw();

    void SwitchTo(StateType l_state);
    void Remove(StateType l_state);
    void ProcessRequests();

    StateType GetCurrentState() const;
    bool HasState(StateType l_state) const;

    SharedContext& GetContext();

private:
    template<typename StateImpl> void RegisterState(StateType l_stateType);

private:
    SharedContext m_context;
    StateContainer m_states;
    StateFactory m_stateFactory;
    StateTypeContainer m_toBeRemoved;
};

#endif