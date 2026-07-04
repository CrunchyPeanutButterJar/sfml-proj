#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/WindowBase.hpp>
#include <eventmanager.hpp>

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

bool operator==(ClosedEvent, ClosedEvent) { return true; }
bool operator==(MouseMovedEvent, MouseMovedEvent) { return true;}

using SimplifiedEvent = std::variant<KeyPressedEvent, MouseButtonPressedEvent, MouseMovedEvent, ClosedEvent>;

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

struct hash
{
    size_t operator()(const KeyPressedEvent& l_event) const
    {
        return std::hash<KeyPressedEventEnumType>{}(l_event.get());
    }

    size_t operator()(const MouseButtonPressedEvent& l_event) const
    {
        return std::hash<MouseButtonPressedEventEnumType>{}(l_event.get());
    }

    size_t operator()(const ClosedEvent&) const
    {
        return 1;
    }

    size_t operator()(const MouseMovedEvent&) const
    {
        return 2; 
    }

    size_t operator()(const SimplifiedEvent& l_event) const
    {
        size_t index = l_event.index();
        return std::visit(
        [index](const auto& l_arg)
        {
            std::size_t h1 = hash{}(l_arg);
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
using CallbacksContainer = std::unordered_map<StateType, Callbacks>;

static const std::string BindingsFilePath{Utils::GetConfigDirectory() + "bindings.json"};

std::optional<SerializableBindings> loadFromBindingsFile()
{
    std::ifstream configFile(BindingsFilePath.c_str());
    
    if(!configFile.good())
    {
        configFile.close();
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << configFile.rdbuf();
    std::string jsonString = buffer.str();
    configFile.close();

    auto result = rfl::json::read<SerializableBindings, rfl::AddTagsToVariants>(jsonString);
    ASSERT_NON_FATAL(result, "Failed to read bindings from file {}", BindingsFilePath);
    
    if(!result)
    {
        return std::nullopt;
    }

    LOG("Reading from binding file {}", BindingsFilePath);

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
    const auto& firstBindings = std::any_cast<const SerializableBindings&>(l_first);
    const auto& secondBindings = std::any_cast<const SerializableBindings&>(l_second);

    return firstBindings == secondBindings;
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

    const auto nonCustomizableBindings = buildNonCustomizableBindings();

    if(auto customizedSerializedBindings = loadFromBindingsFile())
    {
        bindings = std::array{*customizedSerializedBindings, nonCustomizableBindings} | std::ranges::views::join | std::ranges::to<std::vector>();
    }
    else
    {
        const auto defaultCustomizableBindings = buildDefaultBindings();
        bindings = std::array{defaultCustomizableBindings, nonCustomizableBindings} | std::ranges::views::join | std::ranges::to<std::vector>();

        std::ofstream configFile(BindingsFilePath.data());

        const auto jsonString = rfl::json::write < rfl::AddTagsToVariants>(defaultCustomizableBindings);
        configFile << jsonString;
        ASSERT_NON_FATAL(!configFile.fail(), "Failed to write default bindings to file {}", BindingsFilePath);
        configFile.close();
    }
}

bool EventManager::AddCallback(StateType l_state, const std::string& l_action, Callback l_callback)
{
    auto& callbacksContainer = std::get<2>(m_impl->m_tuple);
    auto& callbacks = callbacksContainer[l_state];

    if(callbacks.find(l_action) != callbacks.end())
    {
        return false; // Callback already exists for this action.
    }

    callbacks[l_action] = std::move(l_callback);
    return true;
}

void EventManager::RemoveCallback(StateType l_state, const std::string& l_action)
{
    auto& callbacksContainer = std::get<2>(m_impl->m_tuple);
    auto& callbacks = callbacksContainer[l_state];

    callbacks.erase(l_action);
}

void EventManager::HandleEvent(const sf::Event& l_event)
{
    auto& actualEvents = std::get<1>(m_impl->m_tuple);
    actualEvents.push_back(l_event);
}

void EventManager::HandleRealtimeEvents()
{
    using Set = std::unordered_set<SimplifiedEvent, hash>;

    auto& [bindings, actualEvents, _] = m_impl->m_tuple;
    auto uniqueBindings = bindings | std::ranges::views::transform([](const auto& l_pair){return std::get<1>(l_pair);}) | std::ranges::views::join | std::ranges::to<Set>();

    for(const auto& binding : uniqueBindings)
    {
        std::visit(overloaded{
        [&actualEvents](const KeyPressedEvent& l_event)
        {
            if(sf::Keyboard::isKeyPressed((sf::Keyboard::Key)l_event.get()))
            {
                sf::Event rtEvent;
                rtEvent.type = sf::Event::KeyPressed;
                rtEvent.key.code  = (sf::Keyboard::Key)l_event.get();
                actualEvents.emplace_back(std::move(rtEvent));
            }
        },
        [&actualEvents](const MouseButtonPressedEvent& l_event)
        {
            if(sf::Mouse::isButtonPressed((sf::Mouse::Button)l_event.get()))
            {
                sf::Event rtEvent;
                rtEvent.type = sf::Event::MouseButtonPressed;
                rtEvent.mouseButton.button = (sf::Mouse::Button)l_event.get();
                actualEvents.emplace_back(std::move(rtEvent));
            }
        },
        [](const auto&){}}, binding);
    }
}

static bool simplifiedEventMatchesActualEvent(const SimplifiedEvent& l_simpleEvent, const sf::Event& l_event)
{
  return std::visit(overloaded
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

void EventManager::Update(StateType l_state, const sf::WindowBase& l_window)
{
    HandleRealtimeEvents();
    auto& [bindings, actualEvents, callbacksContainer] = m_impl->m_tuple;

    for(const auto& [action, binding] : bindings)
    {
        if(std::all_of(binding.begin(), binding.end(), [&actualEvents](const auto& l_expectedEvent){return simplifiedEventMatchesActualEvent(l_expectedEvent, actualEvents);}))
        {
            const auto& stateCallbacks = callbacksContainer[l_state];
            auto it = stateCallbacks.find(action);
            if(it != stateCallbacks.end())
            {
                it->second(l_window);
            }

            const auto& otherCallbacks = callbacksContainer[StateType{0}];
            auto otherIt = otherCallbacks.find(action);
            if(otherIt != otherCallbacks.end())
            {
                otherIt->second(l_window);
            }
        }
    }

    actualEvents.clear();
}

EventManager::~EventManager() = default;

EventManager::EventManager(EventManager&&) = default;
EventManager& EventManager::operator=(EventManager&&) = default;