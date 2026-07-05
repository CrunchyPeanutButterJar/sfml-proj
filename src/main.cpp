#include <game.hpp>
#include <utils/assert.hpp>
#include <utils/utilities.hpp>

auto main(int /*argc*/, char* /*argv*/[]) -> int
{
    Game game;

    #ifdef _WIN32
    LOG("Running On Windows!");
    #elif __linux__
    LOG("Running on linux!");
    #else
    #error "Unknown OS!"
    #endif

    LOG("Current working directory : {}", utils::getWorkingDirectory());

    while(!game.getWindow()->isDone())
    {
        //Game loop
        game.update();
        game.render();
        game.lateUpdate();
    }

    return 0;
}