#include <ecs/messaging/communicator.hpp>
#include <algorithm>

using namespace ecs::messaging;

auto Communicator::addObserver(Observer* l_observer) -> bool
{
    return m_observers.insert(l_observer).second;
}

auto Communicator::removeObserver(Observer* l_observer) -> bool
{
    auto itr = std::find(m_observers.begin(), m_observers.end(), l_observer);
    if(itr == m_observers.end())
    {
        return false;
    }
    m_observers.erase(itr);
    return true;
}

auto Communicator::hasObserver(const Observer* l_observer) const -> bool
{
  auto itr = std::find(m_observers.begin(), m_observers.end(), l_observer);
  return itr != m_observers.end();
}

void Communicator::broadcast(const Message &l_message) const 
{
  for (auto *observer : m_observers) {
    observer->notify(l_message);
  }
}