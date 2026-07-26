#ifndef CORE_EVENT_MANAGER_HPP
#define CORE_EVENT_MANAGER_HPP

#include "core/gui/GUI_event.hpp"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowBase.hpp>

#include <core/event_manager.fwd.hpp>
#include <core/state/statemanager.fwd.hpp>

#include <any>

#include <functional>
#include <memory>
#include <string>

namespace core
{

struct EventDetails
{
    std::optional<sf::Vector2i> m_newMousePos;
    std::optional<float>        m_scrollWheelDelta;
    std::optional<int>          m_mouseButton;
    std::optional<char>         m_enteredText;
    bool                        m_realtimeContribution{false};
};

using Callback = std::function<void(const EventDetails&)>;

class EventManager
{
  public:
    void handleEvent(const sf::Event& l_event);
    void handleEvent(const core::gui::GUI_Event& l_event);
    void update(state::StateType l_state);

    auto addCallback(state::StateType l_state, const std::string& l_action,
                     Callback l_callback) -> bool;
    void removeCallback(state::StateType l_state, const std::string& l_action);

    EventManager();
    ~EventManager();

    EventManager(const EventManager&)                    = delete;
    auto operator=(const EventManager&) -> EventManager& = delete;

    EventManager(EventManager&&) noexcept;
    auto operator=(EventManager&&) noexcept -> EventManager&;

  private:
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