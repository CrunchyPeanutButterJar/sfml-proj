#include <ecs/messaging/communicator.hpp>
#include <algorithm>

bool Communicator::addObserver(Observer* l_observer)
{
    return m_observers.insert(l_observer).second;
}

bool Communicator::removeObserver(Observer* l_observer)
{
    auto itr = std::find(m_observers.begin(), m_observers.end(), l_observer);
    if(itr == m_observers.end())
    {
        return false;
    }
    m_observers.erase(itr);
    return true;
}

bool Communicator::hasObserver(const Observer* l_observer) const
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