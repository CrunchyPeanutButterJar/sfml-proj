#include <eventmanager.hpp>

#include <iostream>
#include <fstream>

#include <assert.hpp>

#include <array>
#include <phatomtype.hpp>
#include <string>
#include <tuple>
#include <ranges>
#include <optional>
#include <unordered_map>
#include <variant>
#include <sstream>

#include <rfl/json.hpp>
#include <rfl/rfl.hpp>

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

using BindingsAndEvents = std::vector<std::tuple<Binding, Events>>;

using Callbacks = std::unordered_map<std::string, Callback>;
using CallbacksContainer = std::unordered_map<StateType, Callbacks>;

static constexpr std::string_view BindingsFilePath{"bindings.json"};

std::optional<SerializableBindings> loadFromBindingsFile()
{
    std::ifstream configFile(BindingsFilePath.data());
    
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
    LOG_ERROR(result, "Failed to read bindings from file %s", BindingsFilePath);
    
    if(!result)
    {
        return std::nullopt;
    }

    return result.value();
}

BindingsAndEvents toBindingsAndEvents(const SerializableBindings& bindings)
{
    BindingsAndEvents result;

    for(const auto& [action, events] : bindings)
    {
        result.emplace_back(std::make_tuple(action, events), Events{});
    }

    return result;
}

SerializableBindings toSerializableBindings(const BindingsAndEvents& bindingsAndEvents)
{
    std::vector<Binding> serializableBindings;
    std::ranges::copy( bindingsAndEvents | std::views::transform([](const auto& be) { return std::get<0>(be);}), std::back_inserter(serializableBindings));
    return serializableBindings;
    // return bindingsAndEvents | std::views::transform([](const auto& be) { return std::get<0>(be);}) | std::ranges::to<std::vector>();
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
    return buildDefaultBindings();
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

class EventManager::Impl : public std::pair<BindingsAndEvents, CallbacksContainer> {};

EventManager::EventManager() : m_impl(std::make_unique<Impl>())
{
    auto& [bindingsAndEvents, _] = *m_impl;
    const auto nonCustomizableBindings = buildNonCustomizableBindings();

    if(auto customizedSerializedBindings = loadFromBindingsFile())
    {
        auto& serializedBindings = *customizedSerializedBindings;
        for(const auto& defaultBinding : nonCustomizableBindings)
        {
          serializedBindings.emplace_back(defaultBinding);
        }

        bindingsAndEvents = toBindingsAndEvents(serializedBindings);
    }
    else
    {
        const auto defaultCustomizableBindings = buildDefaultBindings();
        auto defaultBindings = defaultCustomizableBindings;
        for(const auto& defaultBinding : nonCustomizableBindings)
        {
            defaultBindings.emplace_back(defaultBinding);
        }

        bindingsAndEvents = toBindingsAndEvents(defaultBindings);
        std::ofstream configFile(BindingsFilePath.data());

        const auto jsonString = rfl::json::write < rfl::AddTagsToVariants>(defaultCustomizableBindings);
        configFile << jsonString;
        LOG_ERROR(!configFile.fail(), "Failed to write default bindings to file %s", BindingsFilePath);
        configFile.close();
    }
}

bool EventManager::AddCallback(StateType l_state, const std::string& l_action, Callback l_callback)
{
    auto& [_, callbacksContainer] = *m_impl;
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
    auto& [_, callbacksContainer] = *m_impl;
    auto& callbacks = callbacksContainer[l_state];

    callbacks.erase(l_action);
}

template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

void EventManager::HandleEvent(const sf::Event& l_event)
{
    auto& [bindingsAndActualEvents, _] = *m_impl;

    for(auto& [binding, actualEvents] : bindingsAndActualEvents)
    {
        //event matches
        const auto& [_, events] = binding;

        for(const auto& event : events)
        {
            std::visit(overloaded{
            [&](KeyPressedEvent key)
            {
                if(l_event.type == sf::Event::KeyPressed && (l_event.key.code == key.get() || key.get() == sf::Keyboard::Unknown))
                {
                    actualEvents.push_back(l_event);
                }
            },
            [&](MouseButtonPressedEvent mouseButton)
            {
                if(l_event.type == sf::Event::MouseButtonPressed && l_event.mouseButton.button == mouseButton.get())
                {
                    actualEvents.push_back(l_event);
                }
            },
            [&](ClosedEvent)
            {
                if(l_event.type == sf::Event::Closed)
                {
                    actualEvents.push_back(l_event);
                }
            },
            [&](MouseMovedEvent)
            {
                if(l_event.type == sf::Event::MouseMoved)
                {
                    actualEvents.push_back(l_event);
                }
            }}, event);
        }
    }
}

void EventManager::Update(StateType l_state)
{
    auto& [bindingsAndActualEvents, callbacksContainer] = *m_impl;

    for(auto& [binding, actualEvents] : bindingsAndActualEvents)
    {
        const auto& [action, events] = binding;

        if(events.size() == actualEvents.size())
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
        }
    }

    // clear actual events after processing
    for(auto& [_, actualEvents] : bindingsAndActualEvents)
    {
        actualEvents.clear();
    }
}

EventManager::~EventManager() = default;

EventManager::EventManager(EventManager&&) = default;
EventManager& EventManager::operator=(EventManager&&) = default;