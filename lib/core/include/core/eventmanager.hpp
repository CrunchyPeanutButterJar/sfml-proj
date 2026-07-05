#ifndef CORE_EVENTMANAGER_HPP
#define CORE_EVENTMANAGER_HPP

#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowBase.hpp>

#include <core/eventmanager.fwd.hpp>

#include <any>

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace core
{

using Callback = std::function<void(const sf::WindowBase&)>;

class EventManager
{
public:
    using StateType = unsigned int;


    void handleEvent(const sf::Event& l_event);
    void update(StateType l_state, const sf::WindowBase& l_wind);

    bool addCallback(StateType l_state, const std::string& l_action, Callback l_callback);
    void removeCallback(StateType l_state, const std::string& l_action);

    EventManager();
    ~EventManager();

    EventManager(const EventManager&) = delete;
    EventManager& operator=(const EventManager&) = delete;

    EventManager(EventManager&&) noexcept;
    EventManager& operator=(EventManager&&) noexcept;

private:
    void handleRealtimeEvents();


    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

//exposed for tests
std::any buildBindings();
std::any deserializeBindings(const std::string& l_jsonString);
std::string serializeBindings(const std::any& l_serializableBindings);
bool bindingsAreEquivalent(const std::any& l_first, const std::any& l_second);
}

#endif