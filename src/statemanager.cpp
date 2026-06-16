#include <statemanager.hpp>

#include <gamestate.hpp>
#include <window.hpp>

#include <algorithm>

void StateManager::SwitchTo(StateType l_state)
{
    auto itr = std::find_if(m_states.begin(), m_states.end(), [l_state](const auto& el){ return el.first == l_state; });
    
    if(!m_states.empty()) m_states.back().second->Deactivate();
    
    if (itr == m_states.end())
    {
        itr = m_states.insert(m_states.end(), {l_state, m_stateFactory[l_state](this)});
    }

    std::iter_swap(itr, m_states.end() - 1);
    auto& currentState = m_states.back().second;
    currentState->Activate();
    GetContext().m_window.GetRenderWindow()->setView(currentState->GetView());
}

StateType StateManager::GetCurrentState() const
{
    return m_states.back().first;
}

void StateManager::Update(const sf::Time& l_elapsed)
{
    auto ritr = std::find_if(m_states.rbegin(), m_states.rend(), [](const auto& el){ return !el.second->IsTranscendent(); });
    if(ritr == m_states.rend())
    {
        return;
    }

    int rIndex = std::distance(m_states.rbegin(), ritr);
    auto itr = std::prev(m_states.end(), rIndex + 1);
    for(; itr != m_states.end(); ++itr)
    {
        itr->second->Update(l_elapsed);
    }
}

void StateManager::Draw()
{
    auto ritr = std::find_if(m_states.rbegin(), m_states.rend(), [](const auto& el){ return !el.second->IsTransparent(); });
    if(ritr == m_states.rend())
    {
        return;
    }

    int rIndex = std::distance(m_states.rbegin(), ritr);
    auto itr = std::prev(m_states.end(), rIndex + 1);
    for(; itr != m_states.end(); ++itr)
    {
        GetContext().m_window.GetRenderWindow()->setView(itr->second->GetView());
        itr->second->Draw();
    }
}

void StateManager::Remove(StateType l_state)
{
    m_toBeRemoved.insert(l_state);
}

void StateManager::ProcessRequests()
{
    for(auto toRemove : m_toBeRemoved)
    {
        auto itr = std::find_if(m_states.begin(), m_states.end(), [toRemove](const auto& el){ return el.first == toRemove; });
        if(itr != m_states.end())
        {
            m_states.erase(itr);
        }
    }
}

template<typename StateImpl>
void StateManager::RegisterState(StateType l_stateType)
{
    m_stateFactory[l_stateType] = [](StateManager* stateManager) -> StatePtr 
    {
        auto state =  std::make_unique<StateImpl>(*stateManager); 
        state->m_view = stateManager->GetContext().m_window.GetRenderWindow()->getDefaultView();
        state->OnCreate();

        return state;
    };
}

StateManager::StateManager(SharedContext l_sharedContext) : m_context{std::move(l_sharedContext)}
{
    RegisterState<GameState>(StateType::Game);
}

SharedContext& StateManager::GetContext()
{
    return m_context;
}

bool StateManager::HasState(StateType l_state) const
{
    return std::any_of(m_states.begin(), m_states.end(), [l_state](const auto& el){ return el.first == l_state; });
}