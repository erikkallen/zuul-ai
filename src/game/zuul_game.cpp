#include <game/zuul_game.hpp>
#include <memory>

namespace zuul
{

    bool ZuulGame::initialize(int windowWidth, int windowHeight, const ::std::string &windowTitle)
    {
        if (!Game::initialize(windowWidth, windowHeight, windowTitle))
        {
            return false;
        }

        mTileMap = ::std::make_unique<TileMap>();
        if (!mTileMap->loadFromFile("assets/home.tmj", "assets/map_tiles.tsj", getRenderer()))
        {
            return false;
        }

        mPlayer = ::std::make_unique<Player>();
        if (!mPlayer->initialize(getRenderer()))
        {
            return false;
        }

        // Set initial player position to center of screen
        mPlayer->setPosition(
            (windowWidth - 32) / 2.0f, // 32 is player width
            (windowHeight - 32) / 2.0f // 32 is player height
        );

        return true;
    }

    void ZuulGame::update(float deltaTime)
    {
        mPlayer->update(deltaTime);
    }

    void ZuulGame::render()
    {
        mTileMap->render(getRenderer());
        mPlayer->render(getRenderer());
    }

} // namespace zuul