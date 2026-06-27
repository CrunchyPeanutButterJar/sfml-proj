#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <SFML/Window/Event.hpp>

#include <eventmanager.fwd.hpp>
#include <statemanager.hpp>

#include <any>

#include <functional>
#include <memory>
#include <string>
#include <vector>

using Events = std::vector<sf::Event>;
using Callback = std::function<void(const Events&)>;

class EventManager
{
public:
    void HandleEvent(const sf::Event& l_event);
    void Update(StateType l_state);

    bool AddCallback(StateType l_state, const std::string& l_action, Callback l_callback);
    void RemoveCallback(StateType l_state, const std::string& l_action);

    EventManager();
    ~EventManager();

    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    EventManager(EventManager&&);
    EventManager& operator=(EventManager&&);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

//exposed for tests
std::any buildBindings();
std::any deserializeBindings(const std::string& l_jsonString);
std::string serializeBindings(const std::any& l_serializableBindings);
bool bindingsAreEquivalent(const std::any& l_first, const std::any& l_second);

#endif