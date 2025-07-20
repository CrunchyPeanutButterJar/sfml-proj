#include <eventmanager.hpp>

#include <phatomtype.hpp>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>

using KeyPressedEvent = PhantomType<sf::Keyboard::Key, struct KeyPressedEventTag>;
using MouseButtonPressedEvent = PhantomType<sf::Mouse::Button, struct MouseButtonPressedEventTag>;
struct ClosedEvent{};

using SimplifiedEvent = std::variant<KeyPressedEvent, MouseButtonPressedEvent, ClosedEvent>;

using SimplifiedEvents = std::vector<SimplifiedEvent>;

using Binding = std::tuple<std::string, SimplifiedEvents, Events>;
using Bindings = std::vector<Binding>;

using Callbacks = std::unordered_map<std::string, Callback>;

class EventManager::Impl : public std::pair<Bindings, Callbacks> {};

EventManager::EventManager() : m_impl(std::make_unique<Impl>())
{
    auto& [bindings, _] = *m_impl;

    bindings.emplace_back("MoveUp", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Up}}, Events{});
    bindings.emplace_back("MoveDown", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Down}}, Events{});
    bindings.emplace_back("MoveRight", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Right}}, Events{});
    bindings.emplace_back("MoveLeft", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Left}}, Events{});

    bindings.emplace_back("MoveUp", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::W}}, Events{});
    bindings.emplace_back("MoveDown", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::S}}, Events{});
    bindings.emplace_back("MoveRight", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::D}}, Events{});
    bindings.emplace_back("MoveLeft", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::A}}, Events{});

    bindings.emplace_back("Close", SimplifiedEvents{ClosedEvent{}}, Events{});
    bindings.emplace_back("Close", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Escape}}, Events{});

    bindings.emplace_back("ToggleFullscreen", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::F5}}, Events{});
}

bool EventManager::AddCallback(const std::string& l_action, Callback l_callback)
{
    auto& [_, callbacks] = *m_impl;

    if(callbacks.find(l_action) != callbacks.end())
    {
        return false; // Callback already exists for this action.
    }

    callbacks[l_action] = std::move(l_callback);
    return true;
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
            }}, event);
        }
    }
}

void EventManager::Update()
{
    auto& [bindings, callbacks] = *m_impl;

    for(auto& binding : bindings)
    {
        const auto& [action, events, _] = binding;
        auto& [__, ___, actualEvents] = binding;

        if(events.size() == actualEvents.size())
        {
            callbacks[action](actualEvents);
            actualEvents.clear(); // Clear events after processing.
        }
    }
}

EventManager::~EventManager() = default;

EventManager::EventManager(EventManager&&) = default;
EventManager& EventManager::operator=(EventManager&&) = default;