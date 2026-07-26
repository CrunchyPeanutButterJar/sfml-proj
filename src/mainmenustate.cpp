#include <functional>
#include <mainmenustate.hpp>

#include <core/bindings.hpp>
#include <core/event_manager.hpp>
#include <core/gui/GUI_manager.hpp>
#include <core/shared_context.hpp>
#include <core/state/statemanager.hpp>
#include <core/window.hpp>

template class core::RegisterBinding<
    BINDING("MainMenu_Play", core::GuiEventClick{"MainMenu", "Play"}), core::NonCustomizable>;
template class core::RegisterBinding<
    BINDING("MainMenu_Quit", core::GuiEventClick{"MainMenu", "Quit"}), core::NonCustomizable>;

MainMenuState::MainMenuState(core::state::StateManager& l_stateManager) : BaseState(l_stateManager)
{
    auto& gui_manager = m_stateManager.getContext()->m_guiManager;
    gui_manager.loadInterface(StateType::MainMenu, "MainMenu.interface", "MainMenu");
    auto* interface = gui_manager.getInterface(StateType::MainMenu, "MainMenu");
    interface->setPosition({0, 0});
    // auto* play = interface->getElement("Play");
    // play->setText("Play");
    auto& event_manager = m_stateManager.getContext()->m_eventManager;
    auto* state_manager = &m_stateManager;

    event_manager.addCallback(StateType::MainMenu, "MainMenu_Play", [state_manager](const auto&)
                              { state_manager->switchTo(StateType::Game); });
    event_manager.addCallback(StateType::MainMenu, "MainMenu_Quit", [state_manager](const auto&)
                              { state_manager->getContext()->m_window.setAsDone(); });
}

MainMenuState::~MainMenuState()
{
    auto& event_manager = m_stateManager.getContext()->m_eventManager;

    event_manager.removeCallback(StateType::MainMenu, "MainMenu_Play");
    event_manager.removeCallback(StateType::MainMenu, "MainMenu_Quit");
}