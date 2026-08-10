#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <algorithm>
#include <core/event_manager.hpp>
#include <core/gui/GUI_event.hpp>

#include <fstream>
#include <iostream>

#include <utils/assert.hpp>
#include <utils/utilities.hpp>

#include <array>
#include <optional>
#include <ranges>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>

#include <core/bindings.hpp>

#include <SFML/Window/Event.hpp>

#include <rfl.hpp>
#include <rfl/json.hpp>

using namespace core;

template <class... Ts> struct Overloaded : Ts...
{
    using Ts::operator()...;
};

using Bindings = std::vector<SimplifiedEvents>;

using SerializableBindings = std::unordered_map<std::string, Bindings>;

using ActualEvents    = std::vector<sf::Event>;
using ActualGuiEvents = std::vector<core::gui::GUI_Event>;

using Callbacks          = std::unordered_map<std::string, Callback>;
using CallbacksContainer = std::unordered_map<core::state::StateType, Callbacks>;

static const std::string BINDINGS_FILE_PATH{utils::getConfigDirectory() + "bindings.json"};

auto loadFromBindingsFile() -> std::optional<SerializableBindings>
{
    std::ifstream config_file(BINDINGS_FILE_PATH.c_str());

    if (!config_file.good())
    {
        config_file.close();
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << config_file.rdbuf();
    std::string json_string = buffer.str();
    config_file.close();

    auto result = rfl::json::read<SerializableBindings, rfl::AddTagsToVariants>(json_string);
    ASSERT_NON_FATAL(result, "Failed to read bindings from file {}", BINDINGS_FILE_PATH);

    if (!result)
    {
        return std::nullopt;
    }

    LOG("Reading from binding file {}", BINDINGS_FILE_PATH);

    return result.value();
}

using Registry = std::vector<BindingsRegisterFn>;

static Registry s_default_customizable_bindings_registry;
static Registry s_non_customizable_bindings_registry;

void core::registerDefaultCustomizableBinding(BindingsRegisterFn l_fn)
{
    s_default_customizable_bindings_registry.push_back(l_fn);
}

void core::registerNonCustomizableBinding(BindingsRegisterFn l_fn)
{
    s_non_customizable_bindings_registry.push_back(l_fn);
}

auto buildDefaultBindings() -> SerializableBindings
{
    SerializableBindings bindings;

    for (auto fn : s_default_customizable_bindings_registry)
    {
        auto [action, events] = fn();
        bindings[action].push_back(events);
    }

    return bindings;
}

auto buildNonCustomizableBindings() -> SerializableBindings
{
    SerializableBindings bindings;

    bindings["Key_Escape"].push_back(SimplifiedEvents{KeyPressed{sf::Keyboard::Escape}});
    bindings["Key_TextEntered"].push_back(SimplifiedEvents{TextEntered{}});

    bindings["Mouse_Moved"].push_back(SimplifiedEvents{MouseMoved{}});
    bindings["Mouse_ButtonPressedLeft"].push_back(
        SimplifiedEvents{MouseButtonPressed{sf::Mouse::Left}});
    bindings["Mouse_ButtonPressedRight"].push_back(
        SimplifiedEvents{MouseButtonPressed{sf::Mouse::Right}});
    bindings["Mouse_ButtonReleasedLeft"].push_back(
        SimplifiedEvents{MouseButtonReleased{sf::Mouse::Left}});
    bindings["Mouse_ButtonReleasedRight"].push_back(
        SimplifiedEvents{MouseButtonReleased{sf::Mouse::Right}});
    bindings["Mouse_WheelScrolled"].push_back(SimplifiedEvents{MouseWheelScrolled{}});

    bindings["Window_Close"].push_back(SimplifiedEvents{Closed{}});
    bindings["Window_ToggleFullscreen"].push_back(SimplifiedEvents{KeyPressed{sf::Keyboard::F5}});

    for (auto fn : s_non_customizable_bindings_registry)
    {
        auto [action, events] = fn();
        bindings[action].push_back(events);
    }

    return bindings;
}

struct EventManager::Impl
{
    std::tuple<SerializableBindings, ActualEvents, ActualGuiEvents, CallbacksContainer> m_tuple;
};

EventManager::EventManager() : m_impl(std::make_unique<Impl>())
{
    auto& bindings = std::get<0>(m_impl->m_tuple);
    std::get<1>(m_impl->m_tuple).reserve(128);

    const auto NonCustomizableBindings = buildNonCustomizableBindings();

    if (auto customized_serialized_bindings = loadFromBindingsFile())
    {
        bindings = std::array{*customized_serialized_bindings, NonCustomizableBindings} |
                   std::ranges::views::join | std::ranges::to<std::unordered_map>();
    }
    else
    {
        const auto DefaultCustomizableBindings = buildDefaultBindings();
        bindings = std::array{DefaultCustomizableBindings, NonCustomizableBindings} |
                   std::ranges::views::join | std::ranges::to<std::unordered_map>();

        std::ofstream config_file(BINDINGS_FILE_PATH.data());

        const auto JsonString =
            rfl::json::write<rfl::AddTagsToVariants>(DefaultCustomizableBindings);
        config_file << JsonString;
        ASSERT_NON_FATAL(!config_file.fail(), "Failed to write default bindings to file {}",
                         BINDINGS_FILE_PATH);
        config_file.close();
    }
}

auto EventManager::addCallback(core::state::StateType l_state, const std::string& l_action,
                               Callback l_callback) -> bool
{
    auto& callbacks_container = std::get<3>(m_impl->m_tuple);
    auto& callbacks           = callbacks_container[l_state];

    if (callbacks.find(l_action) != callbacks.end())
    {
        return false; // Callback already exists for this action.
    }

    callbacks[l_action] = std::move(l_callback);
    return true;
}

void EventManager::removeCallback(core::state::StateType l_state, const std::string& l_action)
{
    auto& callbacks_container = std::get<3>(m_impl->m_tuple);
    auto& callbacks           = callbacks_container[l_state];

    callbacks.erase(l_action);
}

void EventManager::handleEvent(const sf::Event& l_event)
{
    auto& actual_events = std::get<1>(m_impl->m_tuple);
    actual_events.push_back(l_event);
}

void EventManager::handleEvent(const core::gui::GUI_Event& l_event)
{
    auto& actual_events = std::get<2>(m_impl->m_tuple);
    actual_events.push_back(l_event);
}

static auto simplifiedEventMatchesActualEvent(const SimplifiedEvent& l_simpleEvent,
                                              const sf::Event&       l_event,
                                              core::EventDetails&    l_details) -> bool
{
    return std::visit(
        Overloaded{[&](KeyPressed l_simpleEvent) {
                       return l_event.type == sf::Event::KeyPressed &&
                              l_simpleEvent.get() == l_event.key.code;
                   },
                   [&](KeyReleased l_simpleEvent) {
                       return l_event.type == sf::Event::KeyReleased &&
                              l_simpleEvent.get() == l_event.key.code;
                   },
                   [&](TextEntered)
                   {
                       if (l_event.type == sf::Event::TextEntered)
                       {
                           l_details.m_enteredText = (char)l_event.text.unicode;
                           return true;
                       }
                       return false;
                   },
                   [&](MouseButtonPressed l_simpleEvent)
                   {
                       if (l_event.type == sf::Event::MouseButtonPressed &&
                           l_simpleEvent.get() == l_event.mouseButton.button)
                       {
                           l_details.m_mouseButton = l_event.mouseButton.button;
                           l_details.m_newMousePos = {l_event.mouseButton.x, l_event.mouseButton.y};
                           return true;
                       }
                       return false;
                   },
                   [&](MouseButtonReleased l_simpleEvent)
                   {
                       if (l_event.type == sf::Event::MouseButtonReleased &&
                           l_simpleEvent.get() == l_event.mouseButton.button)
                       {
                           l_details.m_mouseButton = l_event.mouseButton.button;
                           l_details.m_newMousePos = {l_event.mouseButton.x, l_event.mouseButton.y};
                           return true;
                       }
                       return false;
                   },
                   [&](MouseMoved)
                   {
                       if (l_event.type == sf::Event::MouseMoved)
                       {
                           l_details.m_newMousePos = {l_event.mouseMove.x, l_event.mouseMove.y};
                           return true;
                       }
                       return false;
                   },
                   [&](MouseWheelScrolled)
                   {
                       if (l_event.type == sf::Event::MouseWheelScrolled)
                       {
                           l_details.m_scrollWheelDelta = l_event.mouseWheelScroll.delta;
                           l_details.m_newMousePos      = {l_event.mouseWheelScroll.x,
                                                           l_event.mouseWheelScroll.y};
                           return true;
                       }
                       return false;
                   },
                   [&l_event](Closed) { return l_event.type == sf::Event::Closed; },
                   [](auto&&) { return false; }},

        l_simpleEvent);
}

static auto simplifiedEventMatchesActualEvents(const SimplifiedEvent& l_simpleEvent,
                                               const ActualEvents&    l_events,
                                               core::EventDetails&    l_details) -> bool
{
    return std::ranges::any_of(
        l_events, [&](const sf::Event& l_event)
        { return simplifiedEventMatchesActualEvent(l_simpleEvent, l_event, l_details); });
}

static auto matchesInterfaceAndElement(const GuiEvent&             l_expectedEvent,
                                       const core::gui::GUI_Event& l_actualEvent)
{
    const auto& [cur_interface, element] = l_expectedEvent;
    return l_actualEvent.m_interface == cur_interface && l_actualEvent.m_element == element;
}

static auto simplifiedEventMatchesActualGuiEvent(const SimplifiedEvent&      l_simpleEvent,
                                                 const core::gui::GUI_Event& l_event,
                                                 core::EventDetails&         l_details) -> bool
{
    return std::visit(Overloaded{[&](const GuiEventClick& l_expectedEvent) -> bool
                                 {
                                     if (!matchesInterfaceAndElement(l_expectedEvent, l_event))
                                     {
                                         return false;
                                     }

                                     if (l_event.m_type == core::gui::GUI_EventType::Click)
                                     {
                                         sf::Vector2i mouse_pos(sf::Vector2f{
                                             l_event.m_clickCoords.m_x, l_event.m_clickCoords.m_y});
                                         l_details.m_newMousePos = mouse_pos;
                                         return true;
                                     }
                                     return false;
                                 },
                                 [&](const GuiEventRelease& l_expectedEvent) -> bool
                                 {
                                     if (!matchesInterfaceAndElement(l_expectedEvent, l_event))
                                     {
                                         return false;
                                     }

                                     if (l_event.m_type == core::gui::GUI_EventType::Release)
                                     {
                                         sf::Vector2i mouse_pos(sf::Vector2f{
                                             l_event.m_clickCoords.m_x, l_event.m_clickCoords.m_y});
                                         l_details.m_newMousePos = mouse_pos;
                                         return true;
                                     }
                                     return false;
                                 },
                                 [&](const GuiEventHover& l_expectedEvent) -> bool
                                 {
                                     if (!matchesInterfaceAndElement(l_expectedEvent, l_event))
                                     {
                                         return false;
                                     }

                                     if (l_event.m_type == core::gui::GUI_EventType::Hover)
                                     {
                                         sf::Vector2i mouse_pos(sf::Vector2f{
                                             l_event.m_clickCoords.m_x, l_event.m_clickCoords.m_y});
                                         l_details.m_newMousePos = mouse_pos;
                                         return true;
                                     }
                                     return false;
                                 },
                                 [&](const GuiEventLeave& l_expectedEvent) -> bool
                                 {
                                     if (!matchesInterfaceAndElement(l_expectedEvent, l_event))
                                     {
                                         return false;
                                     }
                                     return l_event.m_type == core::gui::GUI_EventType::Leave;
                                 },
                                 [](auto&&) { return false; }},

                      l_simpleEvent);
}

static auto simplifiedEventMatchesActualGuiEvents(const SimplifiedEvent& l_simpleEvent,
                                                  const ActualGuiEvents& l_events,
                                                  core::EventDetails&    l_details) -> bool
{
    return std::ranges::any_of(
        l_events, [&](const core::gui::GUI_Event& l_event)
        { return simplifiedEventMatchesActualGuiEvent(l_simpleEvent, l_event, l_details); });
}

static auto simplifiedEventMatchesRealtimeInput(const SimplifiedEvent& l_expectedEvent) -> bool
{
    return std::visit(
        Overloaded{[](const KeyPressed& l_event)
                   { return sf::Keyboard::isKeyPressed((sf::Keyboard::Key)l_event.get()); },
                   [](const auto&) { return false; }},
        l_expectedEvent);
}

void EventManager::update(core::state::StateType l_state)
{
    auto& [serialized_bindings, actual_events, actual_gui_events, callbacks_container] =
        m_impl->m_tuple;

    for (const auto& [action, bindings] : serialized_bindings)
    {
        for (const auto& binding : bindings)
        {
            EventDetails details;

            const auto BindingMatchesActualEventFn =
                [&actual_events, &realtime_contribution = details.m_realtimeContribution,
                 &details](const auto& l_expectedEvent)
            {
                return simplifiedEventMatchesActualEvents(l_expectedEvent, actual_events,
                                                          details) ||
                       (realtime_contribution
                            ? simplifiedEventMatchesRealtimeInput(l_expectedEvent)
                            : (realtime_contribution =
                                   simplifiedEventMatchesRealtimeInput(l_expectedEvent)));
            };

            if (std::all_of(binding.begin(), binding.end(),
                            [&](const auto& l_expectedEvent)
                            {
                                return BindingMatchesActualEventFn(l_expectedEvent) ||
                                       simplifiedEventMatchesActualGuiEvents(
                                           l_expectedEvent, actual_gui_events, details);
                            }))
            {
                const auto& state_callbacks = callbacks_container[l_state];
                auto        it              = state_callbacks.find(action);

                if (it != state_callbacks.end())
                {
                    it->second(details);
                }

                const auto& other_callbacks = callbacks_container[0];
                auto        other_it        = other_callbacks.find(action);
                if (other_it != other_callbacks.end())
                {
                    other_it->second(details);
                }
            }
        }
    }

    actual_events.clear();
    actual_gui_events.clear();
}

EventManager::~EventManager() = default;

EventManager::EventManager(EventManager&&) noexcept                    = default;
auto EventManager::operator=(EventManager&&) noexcept -> EventManager& = default;

namespace core
{
auto buildBindings() -> std::any
{
    return std::array{buildDefaultBindings(), buildNonCustomizableBindings()} |
           std::ranges::views::join | std::ranges::to<std::unordered_map>();
}

auto bindingsAreEquivalent(const std::any& l_first, const std::any& l_second) -> bool
{
    const auto& first_bindings  = std::any_cast<const SerializableBindings&>(l_first);
    const auto& second_bindings = std::any_cast<const SerializableBindings&>(l_second);

    return first_bindings == second_bindings;
}

auto deserializeBindings(const std::string& l_jsonString) -> std::any
{
    return rfl::json::read<SerializableBindings, rfl::AddTagsToVariants>(l_jsonString).value();
}

auto serializeBindings(const std::any& l_serializableBindings) -> std::string
{
    return rfl::json::write<rfl::AddTagsToVariants>(
        std::any_cast<const SerializableBindings&>(l_serializableBindings));
}
} // namespace core