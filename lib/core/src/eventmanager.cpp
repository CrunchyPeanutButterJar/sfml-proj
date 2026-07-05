#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <core/eventmanager.hpp>

#include <iostream>
#include <fstream>

#include <utils/assert.hpp>
#include <utils/utilities.hpp>

#include <array>
#include <utils/phantomtype.hpp>
#include <string>
#include <tuple>
#include <ranges>
#include <optional>
#include <unordered_map>
#include <variant>
#include <sstream>

#include <SFML/Window/Event.hpp>

#include <rfl/json.hpp>
#include <rfl.hpp>

using KeyPressedEventEnumType = int;
using MouseButtonPressedEventEnumType = int;

using KeyPressedEvent = PhantomType<KeyPressedEventEnumType, struct KeyPressedEventTag>;
using MouseButtonPressedEvent = PhantomType<KeyPressedEventEnumType, struct MouseButtonPressedEventTag>;
struct ClosedEvent{};
struct MouseMovedEvent{}; 

bool operator==(ClosedEvent /*unused*/, ClosedEvent /*unused*/) { return true; }
bool operator==(MouseMovedEvent /*unused*/, MouseMovedEvent /*unused*/) { return true;}

using SimplifiedEvent = std::variant<KeyPressedEvent, MouseButtonPressedEvent, MouseMovedEvent, ClosedEvent>;

template<class... Ts>
struct Overloaded : Ts... { using Ts::operator()...; };

struct Hash
{
    size_t operator()(const KeyPressedEvent& l_event) const
    {
        return std::hash<KeyPressedEventEnumType>{}(l_event.get());
    }

    size_t operator()(const MouseButtonPressedEvent& l_event) const
    {
        return std::hash<MouseButtonPressedEventEnumType>{}(l_event.get());
    }

    size_t operator()(const ClosedEvent& /*unused*/) const
    {
        return 1;
    }

    size_t operator()(const MouseMovedEvent& /*unused*/) const
    {
        return 2; 
    }

    size_t operator()(const SimplifiedEvent& l_event) const
    {
        size_t index = l_event.index();
        return std::visit(
        [index](const auto& l_arg)
        {
            std::size_t h1 = Hash{}(l_arg);
            std::size_t h2 = std::hash<std::size_t>{}(index);
            return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
        }
        , l_event);
    }
};

using SimplifiedEvents = std::vector<SimplifiedEvent>;

using Binding = std::tuple<std::string, SimplifiedEvents>;
using SerializableBindings = std::vector<Binding>;

using ActualEvents = std::vector<sf::Event>;

using Callbacks = std::unordered_map<std::string, Callback>;
using CallbacksContainer = std::unordered_map<EventManager::StateType, Callbacks>;

static const std::string bindings_file_path{Utils::getConfigDirectory() + "bindings.json"};

std::optional<SerializableBindings> loadFromBindingsFile()
{
    std::ifstream config_file(bindings_file_path.c_str());
    
    if(!config_file.good())
    {
        config_file.close();
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << config_file.rdbuf();
    std::string json_string = buffer.str();
    config_file.close();

    auto result = rfl::json::read<SerializableBindings, rfl::AddTagsToVariants>(json_string);
    ASSERT_NON_FATAL(result, "Failed to read bindings from file {}", bindings_file_path);
    
    if(!result)
    {
        return std::nullopt;
    }

    LOG("Reading from binding file {}", bindings_file_path);

    return result.value();
}

SerializableBindings buildDefaultBindings()
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

SerializableBindings buildNonCustomizableBindings()
{
    SerializableBindings bindings;

    bindings.emplace_back("Key_Escape", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Escape}});

    bindings.emplace_back("Mouse_Moved", SimplifiedEvents{MouseMovedEvent{}});
    bindings.emplace_back("Mouse_ButtonPressed", SimplifiedEvents{MouseButtonPressedEvent{sf::Mouse::Left}});
    bindings.emplace_back("Mouse_ButtonPressed", SimplifiedEvents{MouseButtonPressedEvent{sf::Mouse::Right}});

    bindings.emplace_back("Window_Close", SimplifiedEvents{ClosedEvent{}});
    bindings.emplace_back("Window_ToggleFullscreen", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::F5}});
    
    return bindings;
}

std::any buildBindings()
{
    return std::array{buildDefaultBindings(), buildNonCustomizableBindings()} | std::ranges::views::join | std::ranges::to<std::vector>();
}

bool bindingsAreEquivalent(const std::any& l_first, const std::any& l_second)
{
    const auto& first_bindings = std::any_cast<const SerializableBindings&>(l_first);
    const auto& second_bindings = std::any_cast<const SerializableBindings&>(l_second);

    return first_bindings == second_bindings;
}

std::any deserializeBindings(const std::string &l_jsonString)
{
    return rfl::json::read<SerializableBindings, rfl::AddTagsToVariants>(l_jsonString).value();
}

std::string serializeBindings(const std::any& l_serializableBindings)
{
    return rfl::json::write<rfl::AddTagsToVariants>(std::any_cast<const SerializableBindings&>(l_serializableBindings));
}

struct EventManager::Impl
{
  std::tuple<SerializableBindings, ActualEvents, CallbacksContainer> m_tuple;
};

EventManager::EventManager() : m_impl(std::make_unique<Impl>())
{
    auto& bindings = std::get<0>(m_impl->m_tuple);
    std::get<1>(m_impl->m_tuple).reserve(128);

    const auto non_customizable_bindings = buildNonCustomizableBindings();

    if(auto customized_serialized_bindings = loadFromBindingsFile())
    {
        bindings = std::array{*customized_serialized_bindings, non_customizable_bindings} | std::ranges::views::join | std::ranges::to<std::vector>();
    }
    else
    {
        const auto default_customizable_bindings = buildDefaultBindings();
        bindings = std::array{default_customizable_bindings, non_customizable_bindings} | std::ranges::views::join | std::ranges::to<std::vector>();

        std::ofstream config_file(bindings_file_path.data());

        const auto json_string = rfl::json::write < rfl::AddTagsToVariants>(default_customizable_bindings);
        config_file << json_string;
        ASSERT_NON_FATAL(!config_file.fail(), "Failed to write default bindings to file {}", bindings_file_path);
        config_file.close();
    }
}

bool EventManager::addCallback(StateType l_state, const std::string& l_action, Callback l_callback)
{
    auto& callbacks_container = std::get<2>(m_impl->m_tuple);
    auto& callbacks = callbacks_container[l_state];

    if(callbacks.find(l_action) != callbacks.end())
    {
        return false; // Callback already exists for this action.
    }

    callbacks[l_action] = std::move(l_callback);
    return true;
}

void EventManager::removeCallback(StateType l_state, const std::string& l_action)
{
    auto& callbacks_container = std::get<2>(m_impl->m_tuple);
    auto& callbacks = callbacks_container[l_state];

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
    auto unique_bindings = bindings | std::ranges::views::transform([](const auto& l_pair){return std::get<1>(l_pair);}) | std::ranges::views::join | std::ranges::to<Set>();

    for(const auto& binding : unique_bindings)
    {
        std::visit(Overloaded{
        [&actualEvents](const KeyPressedEvent& l_event)
        {
            if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)l_event.get()))
            {
                sf::Event rt_event{};
                rt_event.type = sf::Event::KeyPressed;
                rt_event.key.code  = (sf::Keyboard::Key)l_event.get();
                actualEvents.emplace_back(std::move(rt_event));
            }
        },
        [&actualEvents](const MouseButtonPressedEvent& l_event)
        {
            if(sf::Mouse::isButtonPressed((sf::Mouse::Button)l_event.get()))
            {
                sf::Event rt_event{};
                rt_event.type = sf::Event::MouseButtonPressed;
                rt_event.mouseButton.button = (sf::Mouse::Button)l_event.get();
                actualEvents.emplace_back(std::move(rt_event));
            }
        },
        [](const auto&){}}, binding);
    }
}

static bool simplifiedEventMatchesActualEvent(const SimplifiedEvent& l_simpleEvent, const sf::Event& l_event)
{
  return std::visit(Overloaded
    {
    [&l_event](KeyPressedEvent l_simpleEvent)
    {
        return l_event.type == sf::Event::KeyPressed && l_simpleEvent.get() == l_event.key.code;
    },
    [&l_event](MouseButtonPressedEvent l_simpleEvent)
    {
        return l_event.type == sf::Event::MouseButtonPressed && l_simpleEvent.get() == l_event.mouseButton.button;
    },
    [&l_event](MouseMovedEvent)
    {
        return l_event.type == sf::Event::MouseMoved;
    },
    [&l_event](ClosedEvent)
    {
        return l_event.type == sf::Event::Closed;
    }
    }, l_simpleEvent);
}

static bool simplifiedEventMatchesActualEvent(const SimplifiedEvent& l_simpleEvent, const ActualEvents& l_events)
{
    return std::any_of(l_events.begin(), l_events.end(), [&l_simpleEvent](const sf::Event& l_event){return simplifiedEventMatchesActualEvent(l_simpleEvent, l_event);});
}

void EventManager::update(StateType l_state, const sf::WindowBase& l_window)
{
    handleRealtimeEvents();
    auto& [bindings, actualEvents, callbacksContainer] = m_impl->m_tuple;

    for(const auto& [action, binding] : bindings)
    {
        if(std::all_of(binding.begin(), binding.end(), [&actualEvents](const auto& l_expectedEvent){return simplifiedEventMatchesActualEvent(l_expectedEvent, actualEvents);}))
        {
            const auto& state_callbacks = callbacksContainer[l_state];
            auto it = state_callbacks.find(action);
            if(it != state_callbacks.end())
            {
                it->second(l_window);
            }

            const auto& other_callbacks = callbacksContainer[StateType{0}];
            auto other_it = other_callbacks.find(action);
            if(other_it != other_callbacks.end())
            {
                other_it->second(l_window);
            }
        }
    }

    actualEvents.clear();
}

EventManager::~EventManager() = default;

EventManager::EventManager(EventManager&&) noexcept = default;
EventManager& EventManager::operator=(EventManager&&) noexcept = default;