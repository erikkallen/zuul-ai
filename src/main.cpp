#include <game/zuul_game.hpp>
#include <iostream>

int main(int argc, char *argv[])
{
    zuul::ZuulGame game;

    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;

    if (!game.initialize(WINDOW_WIDTH, WINDOW_HEIGHT, "Zuul"))
    {
        std::cerr << "Failed to initialize game" << std::endl;
        return 1;
    }

    game.run();

    return 0;
}