#ifndef TEXTBOX_HPP
#define TEXTBOX_HPP

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

using MessageContainer = std::vector<std::string>;

class Textbox
{
public:
    Textbox();
    Textbox(size_t l_visible, int l_charSize, int l_width, sf::Vector2f l_screenPos);
    ~Textbox();

    void Setup(size_t l_visible, int l_charSize, int l_width, sf::Vector2f l_screenPos);
    void Add(std::string l_message);
    void Clear();

    void Render(sf::RenderWindow& l_window);
private:
    MessageContainer m_messages;
    size_t m_numVisible;

    sf::RectangleShape m_backdrop;
    sf::Font m_font;
    sf::Text m_content;
};

#endif