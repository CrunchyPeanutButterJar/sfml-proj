#ifndef ECS_MESSAGING_MESSAGE_HPP
#define ECS_MESSAGING_MESSAGE_HPP

#include <variant>
#include <utility>
#include <SFML/System/Vector2.hpp>

using MessageType = unsigned int;

struct Message
{
    MessageType m_type{};
    int m_sender{-1};
    int m_receiver{-1};

    union
    {
        sf::Vector2f m_2f;
        bool m_bool{};
        int m_int;
    };
};

#endif