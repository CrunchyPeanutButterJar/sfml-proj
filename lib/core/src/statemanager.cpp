#include <algorithm>
#include <core/state/statemanager.hpp>
#include <ranges>

#include <core/window.hpp>

using namespace core::state;

void StateManager::switchTo(StateType l_state)
{
    auto itr = std::find_if(m_states.begin(), m_states.end(),
                            [l_state](const auto& el) { return el.first == l_state; });

    if (!m_states.empty())
        m_states.back().second->deactivate();

    if (itr == m_states.end())
    {
        itr = m_states.insert(m_states.end(), {l_state, m_stateFactory[l_state](this)});
    }

    std::iter_swap(itr, m_states.end() - 1);
    auto& current_state = m_states.back().second;
    current_state->activate();
    getContext()->m_window.getRenderWindow()->setView(current_state->getView());
}

auto StateManager::getCurrentState() const -> StateType
{
    return m_states.back().first;
}

void StateManager::update(const sf::Time& l_elapsed)
{
    auto ritr = std::ranges::find_if(std::ranges::reverse_view(m_states),
                                     [](const auto& el) { return !el.second->isTranscendent(); });
    if (ritr == m_states.rend())
    {
        return;
    }

    int  r_index = std::distance(m_states.rbegin(), ritr);
    auto itr     = std::prev(m_states.end(), r_index + 1);
    for (; itr != m_states.end(); ++itr)
    {
        itr->second->update(l_elapsed);
    }
}

void StateManager::draw()
{
    auto ritr = std::find_if(m_states.rbegin(), m_states.rend(),
                             [](const auto& el) { return !el.second->isTransparent(); });
    if (ritr == m_states.rend())
    {
        return;
    }

    int  r_index = std::distance(m_states.rbegin(), ritr);
    auto itr     = std::prev(m_states.end(), r_index + 1);
    for (; itr != m_states.end(); ++itr)
    {
        getContext()->m_window.getRenderWindow()->setView(itr->second->getView());
        itr->second->draw();
    }
}

void StateManager::remove(StateType l_state)
{
    m_toBeRemoved.insert(l_state);
}

void StateManager::processRequests()
{
    for (auto to_remove : m_toBeRemoved)
    {
        auto itr = std::find_if(m_states.begin(), m_states.end(),
                                [to_remove](const auto& el) { return el.first == to_remove; });
        if (itr != m_states.end())
        {
            m_states.erase(itr);
        }
    }
}

auto StateManager::getContext() -> SharedContext*
{
    return &m_context;
}

auto StateManager::hasState(StateType l_state) const -> bool
{
    return std::any_of(m_states.begin(), m_states.end(),
                       [l_state](const auto& el) { return el.first == l_state; });
}