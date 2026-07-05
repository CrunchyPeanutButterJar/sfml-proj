#ifndef CORE_EVENTMANAGER_HPP
#define CORE_EVENTMANAGER_HPP

#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowBase.hpp>

#include <core/eventmanager.fwd.hpp>

#include <any>

#include <functional>
#include <memory>
#include <string>

namespace core
{

using Callback = std::function<void(const sf::WindowBase&)>;

class EventManager
{
  public:
    using StateType = unsigned int;

    void handleEvent(const sf::Event& l_event);
    void update(StateType l_state, const sf::WindowBase& l_wind);

    auto addCallback(StateType l_state, const std::string& l_action, Callback l_callback) -> bool;
    void removeCallback(StateType l_state, const std::string& l_action);

    EventManager();
    ~EventManager();

    EventManager(const EventManager&)                    = delete;
    auto operator=(const EventManager&) -> EventManager& = delete;

    EventManager(EventManager&&) noexcept;
    auto operator=(EventManager&&) noexcept -> EventManager&;

  private:
    void handleRealtimeEvents();

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

// exposed for tests
auto buildBindings() -> std::any;
auto deserializeBindings(const std::string& l_jsonString) -> std::any;
auto serializeBindings(const std::any& l_serializableBindings) -> std::string;
auto bindingsAreEquivalent(const std::any& l_first, const std::any& l_second) -> bool;
} // namespace core

#endif