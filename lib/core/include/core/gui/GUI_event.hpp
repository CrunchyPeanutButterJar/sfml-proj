#ifndef CORE_GUI_GUI_EVENT_HPP
#define CORE_GUI_GUI_EVENT_HPP

#include <string>

namespace core::gui
{
enum class GUI_EventType
{
    None,
    Click,
    Release,
    Hover,
    Leave
};

struct ClickCoordinates
{
    float m_x, m_y;
};

struct GUI_Event
{
    GUI_EventType m_type;
    const char*   m_element;
    const char*   m_interface;
    union
    {
        ClickCoordinates m_clickCoords;
    };
};
} // namespace core::gui
#endif
