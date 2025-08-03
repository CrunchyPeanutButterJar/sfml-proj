#include <assert.hpp>
#include <game.hpp>
#include <utilities.hpp>

int main(int /*argc*/, char* /*argv*/[])
{
    Game game;

    #ifdef _WIN32
    LOG("Running On Windows!");
    #elif __linux__
    LOG("Running on linux!");
    #else
    #error "Unknown OS!"
    #endif

    LOG("Current working directory : {}", Utils::GetWorkingDirectory());

    while(!game.GetWindow()->IsDone())
    {
        //Game loop
        game.Update();
        game.Render();
        game.LateUpdate();
    }

    return 0;
}