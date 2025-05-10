#include <game.hpp>

int main(int argc, char** argv[])
{
    Game game;
    while(!game.GetWindow()->IsDone())
    {
        game.HandleInput();
        game.Update();
        game.Render();
    }

    return 0;
}