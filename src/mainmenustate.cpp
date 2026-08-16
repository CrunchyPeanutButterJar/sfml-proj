#include <functional>
#include <mainmenustate.hpp>

#include <core/audio/sound_manager.hpp>
#include <core/bindings.hpp>
#include <core/event_manager.hpp>
#include <core/gui/GUI_manager.hpp>
#include <core/shared_context.hpp>
#include <core/state/statemanager.hpp>
#include <core/window.hpp>
#include <utils/assert.hpp>

template class core::RegisterBinding<
    BINDING("MainMenu_Play", core::GuiEventClick{"MainMenu", "Play"}), core::NonCustomizable>;
template class core::RegisterBinding<
    BINDING("MainMenu_Quit", core::GuiEventClick{"MainMenu", "Quit"}), core::NonCustomizable>;

MainMenuState::MainMenuState(core::state::StateManager& l_stateManager)
    : BaseState(l_stateManager), m_gif{m_stateManager.getContext()->m_textureManager}
{
    auto& gui_manager = m_stateManager.getContext()->m_guiManager;
    gui_manager.loadInterface(StateType::MainMenu, "MainMenu.interface", "MainMenu");
    auto* cur_interface = gui_manager.getInterface(StateType::MainMenu, "MainMenu");
    cur_interface->setPosition({0, 0});
    // auto* play = interface->getElement("Play");
    // play->setText("Play");
    auto& event_manager = m_stateManager.getContext()->m_eventManager;
    auto* state_manager = &m_stateManager;

    event_manager.addCallback(StateType::MainMenu, "MainMenu_Play", [state_manager](const auto&)
                              { state_manager->switchTo(StateType::Game); });
    event_manager.addCallback(StateType::MainMenu, "MainMenu_Quit", [state_manager](const auto&)
                              { state_manager->getContext()->m_window.setAsDone(); });

    auto& sound_manager = m_stateManager.getContext()->m_soundManager;
    sound_manager.playMusic("media/audio/mainmenu_theme.mp3", 50.F, true);

    ASSERT(m_gif.loadGif("media/gifs/space1.gifconf"), "");

    auto [width, height]             = m_stateManager.getContext()->m_window.getWindowSize();
    auto [image_width, image_height] = m_gif.getImageSize();

    m_gif.scale(width / image_width, height / image_height);
}

MainMenuState::~MainMenuState()
{
    auto& event_manager = m_stateManager.getContext()->m_eventManager;

    event_manager.removeCallback(StateType::MainMenu, "MainMenu_Play");
    event_manager.removeCallback(StateType::MainMenu, "MainMenu_Quit");
}

void MainMenuState::update(const sf::Time& l_elapsed)
{
    m_gif.update(l_elapsed.asSeconds());
}

void MainMenuState::draw()
{
    m_gif.draw(m_stateManager.getContext()->m_window.getRenderWindow());
}