#include <eventmanager.hpp>

#include <phatomtype.hpp>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>

using KeyPressedEvent = PhantomType<sf::Keyboard::Key, struct KeyPressedEventTag>;
using MouseButtonPressedEvent = PhantomType<sf::Mouse::Button, struct MouseButtonPressedEventTag>;

using SimplifiedEvent = std::variant<KeyPressedEvent, MouseButtonPressedEvent>;

using SimplifiedEvents = std::vector<SimplifiedEvent>;

using Binding = std::tuple<std::string, SimplifiedEvents, Events>;
using Bindings = std::vector<Binding>;

using Callbacks = std::unordered_map<std::string, Callback>;

class EventManager::Impl : public std::pair<Bindings, Callbacks> {};

EventManager::EventManager() : m_impl(std::make_unique<Impl>())
{
    auto& [bindings, _] = *m_impl;

    bindings.emplace_back("Move", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Up}}, 0);
    bindings.emplace_back("Move", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Down}}, 0);
    bindings.emplace_back("Move", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Left}}, 0);
    bindings.emplace_back("Move", SimplifiedEvents{KeyPressedEvent{sf::Keyboard::Right}}, 0);
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

void EventManager::HandleEvent(sf::Event& l_event)
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
                if(l_event.type == sf::Event::KeyPressed && l_event.key.code == key.get())
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
            }
            }, event);
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