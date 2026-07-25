#ifndef CORE_GUI_GUI_MANAGER_HPP
#define CORE_GUI_GUI_MANAGER_HPP

#include "core/event_manager.hpp"
#include <SFML/Graphics.hpp>
#include <core/event_manager.fwd.hpp>
#include <core/shared_context.hpp>
#include <core/state/statemanager.fwd.hpp>

#include <core/gui/GUI_event.hpp>
#include <core/gui/GUI_interface.hpp>
#include <core/gui/GUI_label.hpp>
#include <core/gui/GUI_manager.fwd.hpp>
#include <core/gui/GUI_scrollbar.hpp>
#include <core/gui/GUI_textfield.hpp>

#include <functional>
#include <unordered_map>

namespace core::gui
{
using GUI_Interfaces = std::map<std::string, GUI_InterfacePtr>;
using GUI_Container  = std::unordered_map<state::StateType, GUI_Interfaces>;
using GUI_Events     = std::unordered_map<state::StateType, std::vector<GUI_Event>>;
using GUI_Factory =
    std::unordered_map<GUI_ElementType, std::function<GUI_ElementPtr(GUI_Interface*)>>;

class GUI_Manager
{
    friend class GUI_Interface;

  public:
    GUI_Manager(EventManager* l_evMgr, SharedContext* l_shared);
    ~GUI_Manager();

    auto addInterface(state::StateType l_state, const std::string& l_name) -> bool;
    auto getInterface(state::StateType l_state, const std::string& l_name) -> GUI_Interface*;
    auto removeInterface(state::StateType l_state, const std::string& l_name) -> bool;

    auto loadInterface(state::StateType l_state, const std::string& l_interface,
                       const std::string& l_name) -> bool;

    void setCurrentState(state::StateType l_state);

    auto getContext() -> SharedContext*;

    void defocusAllInterfaces();

    void handleClick(const core::EventDetails& l_details);
    void handleRelease(const core::EventDetails& l_details);
    void handleTextEntered(const core::EventDetails& l_details);

    void addEvent(GUI_Event l_event);
    auto pollEvent(GUI_Event& l_event) -> bool;

    void update(float l_dT);
    void render(sf::RenderWindow* l_wind);

    template <class T> void registerElement(const GUI_ElementType& l_id)
    {
        m_factory[l_id] = [](GUI_Interface* l_owner) -> GUI_ElementPtr
        { return std::make_unique<T>("", l_owner); };
    }

  private:
    auto createElement(const GUI_ElementType& l_id, GUI_Interface* l_owner) -> GUI_ElementPtr;
    static auto loadStyle(const std::string& l_file, GUI_Element* l_element) -> bool;

    GUI_Container          m_interfaces;
    GUI_Events             m_events;
    EventManager*          m_eventMgr;
    SharedContext*         m_context;
    core::state::StateType m_currentState;
    GUI_Factory            m_factory;
};
} // namespace core::gui
#endif
