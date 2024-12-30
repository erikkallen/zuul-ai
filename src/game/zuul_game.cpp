#include "game/zuul_game.hpp"
#include <memory>

namespace zuul
{

    bool ZuulGame::initialize(int windowWidth, int windowHeight, const std::string &windowTitle)
    {
        if (!Game::initialize(windowWidth, windowHeight, windowTitle))
        {
            return false;
        }

        mTileMap = std::make_unique<TileMap>();
        if (!mTileMap->loadFromFile("assets/home.tmj", "assets/map_tiles.tsj", getRenderer()))
        {
            return false;
        }

        return true;
    }

    void ZuulGame::update(float deltaTime)
    {
        // Update game logic here
    }

    void ZuulGame::render()
    {
        mTileMap->render(getRenderer());
    }

} // namespace zuul