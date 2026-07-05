#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <core/eventmanager.hpp>

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
#include <utils/phantomtype.hpp>
#include <variant>

#include <SFML/Window/Event.hpp>

#include <rfl.hpp>
#include <rfl/json.hpp>

using namespace core;

using KeyPressedEventEnumType         = int;
using MouseButtonPressedEventEnumType = int;

using KeyPressedEvent = utils::PhantomType<KeyPressedEventEnumType, struct KeyPressedEventTag>;
using MouseButtonPressedEvent =
    utils::PhantomType<KeyPressedEventEnumType, struct MouseButtonPressedEventTag>;
struct ClosedEvent
{
};
struct MouseMovedEvent
{
};

auto operator==(ClosedEvent /*unused*/, ClosedEvent /*unused*/) -> bool
{
    return true;
}
auto operator==(MouseMovedEvent /*unused*/, MouseMovedEvent /*unused*/) -> bool
{
    return true;
}

using SimplifiedEvent =
    std::variant<KeyPressedEvent, MouseButtonPressedEvent, MouseMovedEvent, ClosedEvent>;

template <class... Ts> struct Overloaded : Ts...
{
    using Ts::operator()...;
};

struct Hash
{
    auto operator()(const KeyPressedEvent& l_event) const -> size_t
    {
        return std::hash<KeyPressedEventEnumType>{}(l_event.get());
    }

    auto operator()(const MouseButtonPressedEvent& l_event) const -> size_t
    {
        return std::hash<MouseButtonPressedEventEnumType>{}(l_event.get());
    }

    auto operator()(const ClosedEvent& /*unused*/) const -> size_t { return 1; }

    auto operator()(const MouseMovedEvent& /*unused*/) const -> size_t { return 2; }

    auto operator()(const SimplifiedEvent& l_event) const -> size_t
    {
        size_t index = l_event.index();
        return std::visit(
            [index](const auto& l_arg)
            {
                std::size_t h1 = Hash{}(l_arg);
                std::size_t h2 = std::hash<std::size_t>{}(index);
                return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
            },
            l_event);
    }
};

using SimplifiedEvents = std::vector<SimplifiedEvent>;

using Binding              = std::tuple<std::string, SimplifiedEvents>;
using SerializableBindings = std::vector<Binding>;

using ActualEvents = std::vector<sf::Event>;

using Callbacks          = std::unordered_map<std::string, Callback>;
using CallbacksContainer = std::unordered_map<EventManager::StateType, Callbacks>;

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

auto buildDefaultBindings() -> SerializableBindings
{
    SerializableBindings bindings;

    bindings.emplace_back("Game_MoveUp", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Up}});
    bindings.emplace_back("Game_MoveDown", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Down}});
    bindings.emplace_back("Game_MoveRight", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Right}});
    bindings.emplace_back("Game_MoveLeft", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Left}});

    bindings.emplace_back("Game_MoveUp", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::W}});
    bindings.emplace_back("Game_MoveDown", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::S}});
    bindings.emplace_back("Game_MoveRight", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::D}});
    bindings.emplace_back("Game_MoveLeft", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::A}});

    return bindings;
}

auto buildNonCustomizableBindings() -> SerializableBindings
{
    SerializableBindings bindings;

    bindings.emplace_back("Key_Escape", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Escape}});

    bindings.emplace_back("Mouse_Moved", SimplifiedEvents{MouseMovedEvent{}});
    bindings.emplace_back("Mouse_ButtonPressed",
                          SimplifiedEvents{MouseButtonPressedEvent{sf::Mouse::Left}});
    bindings.emplace_back("Mouse_ButtonPressed",
                          SimplifiedEvents{MouseButtonPressedEvent{sf::Mouse::Right}});

    bindings.emplace_back("Window_Close", SimplifiedEvents{ClosedEvent{}});
    bindings.emplace_back("Window_ToggleFullscreen",
                          SimplifiedEvents{KeyPressedEvent{sf::Keyboard::F5}});

    return bindings;
}

struct EventManager::Impl
{
    std::tuple<SerializableBindings, ActualEvents, CallbacksContainer> m_tuple;
};

EventManager::EventManager() : m_impl(std::make_unique<Impl>())
{
    auto& bindings = std::get<0>(m_impl->m_tuple);
    std::get<1>(m_impl->m_tuple).reserve(128);

    const auto NonCustomizableBindings = buildNonCustomizableBindings();

    if (auto customized_serialized_bindings = loadFromBindingsFile())
    {
        bindings = std::array{*customized_serialized_bindings, NonCustomizableBindings} |
                   std::ranges::views::join | std::ranges::to<std::vector>();
    }
    else
    {
        const auto DefaultCustomizableBindings = buildDefaultBindings();
        bindings = std::array{DefaultCustomizableBindings, NonCustomizableBindings} |
                   std::ranges::views::join | std::ranges::to<std::vector>();

        std::ofstream config_file(BINDINGS_FILE_PATH.data());

        const auto JsonString =
            rfl::json::write<rfl::AddTagsToVariants>(DefaultCustomizableBindings);
        config_file << JsonString;
        ASSERT_NON_FATAL(!config_file.fail(), "Failed to write default bindings to file {}",
                         BINDINGS_FILE_PATH);
        config_file.close();
    }
}

auto EventManager::addCallback(StateType l_state, const std::string& l_action,
                               Callback l_callback) -> bool
{
    auto& callbacks_container = std::get<2>(m_impl->m_tuple);
    auto& callbacks           = callbacks_container[l_state];

    if (callbacks.find(l_action) != callbacks.end())
    {
        return false; // Callback already exists for this action.
    }

    callbacks[l_action] = std::move(l_callback);
    return true;
}

void EventManager::removeCallback(StateType l_state, const std::string& l_action)
{
    auto& callbacks_container = std::get<2>(m_impl->m_tuple);
    auto& callbacks           = callbacks_container[l_state];

    callbacks.erase(l_action);
}

void EventManager::handleEvent(const sf::Event& l_event)
{
    auto& actual_events = std::get<1>(m_impl->m_tuple);
    actual_events.push_back(l_event);
}

void EventManager::handleRealtimeEvents()
{
    using Set = std::unordered_set<SimplifiedEvent, Hash>;

    auto& [bindings, actualEvents, _] = m_impl->m_tuple;
    auto unique_bindings =
        bindings |
        std::ranges::views::transform([](const auto& l_pair) { return std::get<1>(l_pair); }) |
        std::ranges::views::join | std::ranges::to<Set>();

    for (const auto& binding : unique_bindings)
    {
        std::visit(Overloaded{[&actualEvents](const KeyPressedEvent& l_event)
                              {
                                  if (sf::Keyboard::isKeyPressed((sf::Keyboard::Key)l_event.get()))
                                  {
                                      sf::Event rt_event{};
                                      rt_event.type     = sf::Event::KeyPressed;
                                      rt_event.key.code = (sf::Keyboard::Key)l_event.get();
                                      actualEvents.emplace_back(rt_event);
                                  }
                              },
                              [&actualEvents](const MouseButtonPressedEvent& l_event)
                              {
                                  if (sf::Mouse::isButtonPressed((sf::Mouse::Button)l_event.get()))
                                  {
                                      sf::Event rt_event{};
                                      rt_event.type = sf::Event::MouseButtonPressed;
                                      rt_event.mouseButton.button =
                                          (sf::Mouse::Button)l_event.get();
                                      actualEvents.emplace_back(rt_event);
                                  }
                              },
                              [](const auto&) {}},
                   binding);
    }
}

static auto simplifiedEventMatchesActualEvent(const SimplifiedEvent& l_simpleEvent,
                                              const sf::Event&       l_event) -> bool
{
    return std::visit(
        Overloaded{[&l_event](KeyPressedEvent l_simpleEvent) {
                       return l_event.type == sf::Event::KeyPressed &&
                              l_simpleEvent.get() == l_event.key.code;
                   },
                   [&l_event](MouseButtonPressedEvent l_simpleEvent)
                   {
                       return l_event.type == sf::Event::MouseButtonPressed &&
                              l_simpleEvent.get() == l_event.mouseButton.button;
                   },
                   [&l_event](MouseMovedEvent) { return l_event.type == sf::Event::MouseMoved; },
                   [&l_event](ClosedEvent) { return l_event.type == sf::Event::Closed; }},
        l_simpleEvent);
}

static auto simplifiedEventMatchesActualEvent(const SimplifiedEvent& l_simpleEvent,
                                              const ActualEvents&    l_events) -> bool
{
    return std::any_of(l_events.begin(), l_events.end(), [&l_simpleEvent](const sf::Event& l_event)
                       { return simplifiedEventMatchesActualEvent(l_simpleEvent, l_event); });
}

void EventManager::update(StateType l_state, const sf::WindowBase& l_window)
{
    handleRealtimeEvents();
    auto& [bindings, actualEvents, callbacksContainer] = m_impl->m_tuple;

    for (const auto& [action, binding] : bindings)
    {
        if (std::all_of(binding.begin(), binding.end(),
                        [&actualEvents](const auto& l_expectedEvent) {
                            return simplifiedEventMatchesActualEvent(l_expectedEvent, actualEvents);
                        }))
        {
            const auto& state_callbacks = callbacksContainer[l_state];
            auto        it              = state_callbacks.find(action);
            if (it != state_callbacks.end())
            {
                it->second(l_window);
            }

            const auto& other_callbacks = callbacksContainer[StateType{0}];
            auto        other_it        = other_callbacks.find(action);
            if (other_it != other_callbacks.end())
            {
                other_it->second(l_window);
            }
        }
    }

    actualEvents.clear();
}

EventManager::~EventManager() = default;

EventManager::EventManager(EventManager&&) noexcept                    = default;
auto EventManager::operator=(EventManager&&) noexcept -> EventManager& = default;

namespace core
{
auto buildBindings() -> std::any
{
    return std::array{buildDefaultBindings(), buildNonCustomizableBindings()} |
           std::ranges::views::join | std::ranges::to<std::vector>();
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