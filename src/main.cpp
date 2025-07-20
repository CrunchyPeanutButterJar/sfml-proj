#include <game.hpp>

int main(int argc, char* argv[])
{
    Game game;
    while(!game.GetWindow()->IsDone())
    {
        //Game loop
        game.Update();
        game.Render();
        game.LateUpdate();
    }

    return 0;
}