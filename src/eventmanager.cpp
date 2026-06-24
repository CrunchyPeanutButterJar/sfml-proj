#include <SFML/Window/Keyboard.hpp>
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
    bindings.emplace_back("Game_MoveRight", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Right}, KeyPressedEvent{sf::Keyboard::Space}});
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

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

struct AreEquivalent
{
    bool operator()(const sf::Event::KeyEvent& l_e1, const sf::Event::KeyEvent& l_e2) const
    {
        return  l_e1.code == l_e2.code &&
                l_e1.scancode == l_e2.scancode &&
                l_e1.alt == l_e2.alt &&
                l_e1.control == l_e2.control &&
                l_e1.shift == l_e2.shift &&
                l_e1.system == l_e2.system;
    }

    bool operator()(const sf::Event::MouseButtonEvent& l_e1, const sf::Event::MouseButtonEvent& l_e2) const
    {
        return l_e1.button == l_e2.button;
    }
};

template<typename T>
const T& readEventData(const sf::Event& l_event, T sf::Event::* l_dataMemberAccessor)
{
    return l_event.*l_dataMemberAccessor;
}

static bool isEventOfType(const sf::Event& l_event, sf::Event::EventType l_type)
{
    return l_event.type == l_type;
}

void EventManager::HandleEvent(const sf::Event& l_event)
{
    static constexpr auto findPressedEvent = 
    []<typename T>(ActualEvents& l_events, const sf::Event& l_event, sf::Event::EventType l_eventType, T sf::Event::* l_dataMemberAccessor)
    {
        auto it = std::find_if(l_events.begin(), l_events.end(),
        [&](const auto& l_currentEvent)
        {
            if(isEventOfType(l_currentEvent, l_eventType))
            {
                return AreEquivalent{}(readEventData(l_currentEvent, l_dataMemberAccessor), readEventData(l_event, l_dataMemberAccessor));
            }
            return false;
        });
        return it;
    };

    auto& actualEvents = std::get<1>(m_impl->m_tuple);

    if(isEventOfType(l_event, sf::Event::KeyReleased))
    {
        if(auto it = findPressedEvent(actualEvents, l_event, sf::Event::KeyPressed, &sf::Event::key); it!=actualEvents.end())
        {
            actualEvents.erase(it);
        }
    }
    else if(isEventOfType(l_event, sf::Event::MouseButtonReleased))
    {
        if(auto it = findPressedEvent(actualEvents, l_event, sf::Event::MouseButtonPressed, &sf::Event::mouseButton); it != actualEvents.end())
        {
            actualEvents.erase(it);
        }
    }

    actualEvents.push_back(l_event);
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

using ActualEventsIndices = std::set<size_t, std::greater<size_t>>;

static bool simplifiedEventMatchesActualEvent(const SimplifiedEvent& l_simpleEvent, const ActualEvents& l_events, ActualEventsIndices& l_matchedEventsIds)
{
    for(size_t i = 0; i <l_events.size(); ++i)
    {
        if (simplifiedEventMatchesActualEvent(l_simpleEvent, l_events[i]))
        {
            l_matchedEventsIds.insert(i);
            return true;
        }
    }
    return false;
}

void EventManager::Update(StateType l_state)
{
    auto& [bindings, actualEvents, callbacksContainer] = m_impl->m_tuple;

    ActualEventsIndices toRemove;

    for(const auto& [action, binding] : bindings)
    {
        auto copy = toRemove;
        if(std::all_of(binding.begin(), binding.end(), [&actualEvents, &copy](const auto& l_expectedEvent){return simplifiedEventMatchesActualEvent(l_expectedEvent, actualEvents, copy);}))
        {
            const auto& stateCallbacks = callbacksContainer[l_state];
            auto it = stateCallbacks.find(action);
            if(it != stateCallbacks.end())
            {
                it->second(actualEvents);
            }

            const auto& otherCallbacks = callbacksContainer[StateType{0}];
            auto otherIt = otherCallbacks.find(action);
            if(otherIt != otherCallbacks.end())
            {
                otherIt->second(actualEvents);
            }

            toRemove = copy;
        }
    }

    for(size_t i = 0; i < actualEvents.size(); i++)
    {
        if(actualEvents[i].type != sf::Event::KeyPressed && actualEvents[i].type != sf::Event::MouseButtonPressed)
        {
            toRemove.insert(i);
        }
    }

    for(auto id : toRemove)
    {
        actualEvents.erase(std::next(actualEvents.begin(), id));
    }
}

EventManager::~EventManager() = default;

EventManager::EventManager(EventManager&&) = default;
EventManager& EventManager::operator=(EventManager&&) = default;