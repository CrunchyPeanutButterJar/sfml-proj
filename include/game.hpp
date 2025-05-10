#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>

#include <window.hpp>

class Game
{
public:
    Game();
    ~Game() = default;

    void HandleInput();
    void Update();
    void Render();
    Window* GetWindow();

private:
    Window m_window;

    sf::Texture m_catTexture;
    sf::Sprite m_cat;
};

#endif