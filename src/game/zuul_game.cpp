#include <game/zuul_game.hpp>
#include <memory>

namespace zuul
{

    bool ZuulGame::initialize(int windowWidth, int windowHeight, const ::std::string &windowTitle)
    {
        mWindowWidth = windowWidth;
        mWindowHeight = windowHeight;

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

        // Initialize camera with map dimensions from tilemap
        mCamera = ::std::make_unique<Camera>(
            windowWidth,
            windowHeight,
            mTileMap->getWidth() * mTileMap->getTileWidth(),
            mTileMap->getHeight() * mTileMap->getTileHeight());

        return true;
    }

    void ZuulGame::update(float deltaTime)
    {
        mPlayer->update(deltaTime);
        mTileMap->update(deltaTime); // Update animated tiles

        // Update camera to follow player
        mCamera->update(mPlayer->getX(), mPlayer->getY());

        // Get screen coordinates for player position
        float screenX, screenY;
        mCamera->worldToScreen(mPlayer->getX(), mPlayer->getY(), screenX, screenY);

        // Check map boundaries and adjust player position if needed
        if (screenX < 0)
            mPlayer->setPosition(mCamera->getOffsetX(), mPlayer->getY());
        if (screenY < 0)
            mPlayer->setPosition(mPlayer->getX(), mCamera->getOffsetY());
        if (screenX > mWindowWidth - 32)
            mPlayer->setPosition(mCamera->getOffsetX() + mWindowWidth - 32, mPlayer->getY());
        if (screenY > mWindowHeight - 32)
            mPlayer->setPosition(mPlayer->getX(), mCamera->getOffsetY() + mWindowHeight - 32);
    }

    void ZuulGame::render()
    {
        // Apply camera offset when rendering
        float mapOffsetX = -mCamera->getOffsetX();
        float mapOffsetY = -mCamera->getOffsetY();

        // Render tilemap with offset
        mTileMap->render(getRenderer(), mapOffsetX, mapOffsetY);

        // Get screen coordinates for player
        float screenX, screenY;
        mCamera->worldToScreen(mPlayer->getX(), mPlayer->getY(), screenX, screenY);

        // Set player position to screen coordinates for rendering
        float oldX = mPlayer->getX();
        float oldY = mPlayer->getY();
        mPlayer->setPosition(screenX, screenY);

        // Render player
        mPlayer->render(getRenderer());

        // Restore player's world coordinates
        mPlayer->setPosition(oldX, oldY);
    }

} // namespace zuul