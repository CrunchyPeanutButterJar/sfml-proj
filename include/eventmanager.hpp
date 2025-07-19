#ifndef EVENTMANAGER_HPP
#define EVENTMANAGER_HPP

#include <SFML/Window/Event.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using Events = std::vector<sf::Event>;
using Callback = std::function<void(const Events&)>;

class EventManager
{
public:
    void HandleEvent(sf::Event& l_event);
    void Update();

    bool AddCallback(const std::string& l_action, Callback l_callback);

    EventManager();
    ~EventManager();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

#endif