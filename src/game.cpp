#include <game.hpp>

Game::Game() : m_window{"Chapter 2", sf::Vector2u(1920, 1080)}
{
    m_catTexture.loadFromFile("resources/cat.jpg");
    m_cat.setTexture(m_catTexture);
    m_cat.setOrigin(m_catTexture.getSize().x/2, m_catTexture.getSize().y/2);

    m_cat.setPosition(m_window.GetWindowSize().x/2, m_window.GetWindowSize().y/2);
}

void Game::Update()
{
    m_window.Update();
}

void Game::HandleInput()
{

}

Window* Game::GetWindow()
{
    return &m_window;
}

void Game::Render()
{
    m_window.BeginDraw();
    m_window.Draw(m_cat);
    m_window.EndDraw();
}