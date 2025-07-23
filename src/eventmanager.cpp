#include <eventmanager.hpp>

#include <phatomtype.hpp>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>

using KeyPressedEvent = PhantomType<sf::Keyboard::Key, struct KeyPressedEventTag>;
using MouseButtonPressedEvent = PhantomType<sf::Mouse::Button, struct MouseButtonPressedEventTag>;
struct ClosedEvent{};
struct MouseMovedEvent{};

using SimplifiedEvent = std::variant<KeyPressedEvent, MouseButtonPressedEvent, MouseMovedEvent, ClosedEvent>;

using SimplifiedEvents = std::vector<SimplifiedEvent>;

using Binding = std::tuple<std::string, SimplifiedEvents, Events>;
using Bindings = std::vector<Binding>;

using Callbacks = std::unordered_map<std::string, Callback>;
using CallbacksContainer = std::unordered_map<StateType, Callbacks>;

class EventManager::Impl : public std::pair<Bindings, CallbacksContainer> {};

EventManager::EventManager() : m_impl(std::make_unique<Impl>())
{
    auto& [bindings, _] = *m_impl;

    bindings.emplace_back("Key_Escape", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Escape}}, Events{});

    bindings.emplace_back("Game_MoveUp", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Up}}, Events{});
    bindings.emplace_back("Game_MoveDown", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Down}}, Events{});
    bindings.emplace_back("Game_MoveRight", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Right}}, Events{});
    bindings.emplace_back("Game_MoveLeft", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Left}}, Events{});

    bindings.emplace_back("Game_MoveUp", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::W}}, Events{});
    bindings.emplace_back("Game_MoveDown", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::S}}, Events{});
    bindings.emplace_back("Game_MoveRight", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::D}}, Events{});
    bindings.emplace_back("Game_MoveLeft", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::A}}, Events{});

    bindings.emplace_back("Mouse_Moved", SimplifiedEvents{MouseMovedEvent{}}, Events{});
    bindings.emplace_back("Mouse_ButtonPressed", SimplifiedEvents{MouseButtonPressedEvent{sf::Mouse::Left}}, Events{});
    bindings.emplace_back("Mouse_ButtonPressed", SimplifiedEvents{MouseButtonPressedEvent{sf::Mouse::Right}}, Events{});

    bindings.emplace_back("Window_Close", SimplifiedEvents{ClosedEvent{}}, Events{});
    bindings.emplace_back("Window_ToggleFullscreen", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::F5}}, Events{});
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
    auto& [bindings, _] = *m_impl;

    for(auto& binding : bindings)
    {
        //event matches
        const auto& [_, events, __] = binding;
        for(const auto& event : events)
        {
            std::visit(overloaded{
            [&](KeyPressedEvent key)
            {
                if(l_event.type == sf::Event::KeyPressed && (l_event.key.code == key.get() || key.get() == sf::Keyboard::Unknown))
                {
                    auto& [_, __, actualEvents] = binding;
                    actualEvents.push_back(l_event);
                }
            },
            [&](MouseButtonPressedEvent mouseButton)
            {
                if(l_event.type == sf::Event::MouseButtonPressed && l_event.mouseButton.button == mouseButton.get())
                {
                    auto& [_, __, actualEvents] = binding;
                    actualEvents.push_back(l_event);
                }
            },
            [&](ClosedEvent)
            {
                if(l_event.type == sf::Event::Closed)
                {
                    auto& [_, __, actualEvents] = binding;
                    actualEvents.push_back(l_event);
                }
            },
            [&](MouseMovedEvent)
            {
                if(l_event.type == sf::Event::MouseMoved)
                {
                    auto& [_, __, actualEvents] = binding;
                    actualEvents.push_back(l_event);
                }
            }}, event);
        }
    }
}

void EventManager::Update(StateType l_state)
{
    auto& [bindings, callbacksContainer] = *m_impl;

    for(auto& binding : bindings)
    {
        const auto& [action, events, _] = binding;
        auto& [__, ___, actualEvents] = binding;

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
    for(auto& binding : bindings)
    {
        auto& [_, __, actualEvents] = binding;
        actualEvents.clear();
    }
}

EventManager::~EventManager() = default;

EventManager::EventManager(EventManager&&) = default;
EventManager& EventManager::operator=(EventManager&&) = default;