#ifndef WORLD_HPP
#define WORLD_HPP

#include <SFML/Graphics.hpp>

class World
{
public:
    World(sf::Vector2u l_windSize);
    ~World();

    void Update();
    void Render(sf::RenderWindow &l_window);

  private:
    sf::Vector2u m_windowSize;
};

#endif