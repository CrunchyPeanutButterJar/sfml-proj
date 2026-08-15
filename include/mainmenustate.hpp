#ifndef MAINMENUSTATE_HPP
#define MAINMENUSTATE_HPP

#include <statetype.hpp>

#include <core/state/basestate.hpp>
#include <core/graphics/gif.hpp>

class MainMenuState : public core::state::BaseState
{
  public:
    MainMenuState(core::state::StateManager& l_stateManager);
    ~MainMenuState() override;

    void update(const sf::Time& l_elapsed) override;
    void draw() override;

    void activate() override{};
    void deactivate() override{};

    void onCreate() override{};

    static constexpr StateType TYPE = StateType::MainMenu;
  
  private:
    core::graphics::Gif m_gif;
};

#endif