#include <game/zuul_game.hpp>
#include <SDL2/SDL.h>
#include <iostream>

namespace zuul
{

    bool ZuulGame::initialize(int windowWidth, int windowHeight, const std::string &title)
    {
        if (!Game::initialize(windowWidth, windowHeight, title))
        {
            return false;
        }

        mTileMap = std::make_unique<TileMap>();
        if (!mTileMap->loadFromFile("assets/home.tmj", getRenderer()))
        {
            return false;
        }

        mPlayer = std::make_unique<Player>();
        if (!mPlayer->initialize(getRenderer()))
        {
            return false;
        }

        // Set initial player position
        mPlayer->setPosition(100, 100);

        // Initialize camera
        mCamera = std::make_unique<Camera>(
            windowWidth,
            windowHeight,
            mTileMap->getWidth() * mTileMap->getTileWidth(),
            mTileMap->getHeight() * mTileMap->getTileHeight());

        // Initialize UI
        mUI = std::make_unique<UI>();
        if (!mUI->initialize(getRenderer()))
        {
            return false;
        }

        // Set up item collect callback
        mTileMap->setItemCollectCallback([this](int itemId)
                                         {
            std::cout << "Item collected: " << itemId << std::endl;
            mUI->addCollectedItem(itemId); });

        return true;
    }

    void ZuulGame::update(float deltaTime)
    {
        // Get keyboard state
        const Uint8 *keyState = SDL_GetKeyboardState(nullptr);

        // Toggle debug rendering with F1
        static bool lastF1State = false;
        bool currentF1State = keyState[SDL_SCANCODE_F1];
        if (currentF1State && !lastF1State)
        {
            mDebugRendering = !mDebugRendering;
            mTileMap->setDebugRendering(mDebugRendering);
            mPlayer->setDebugRendering(mDebugRendering);
        }
        lastF1State = currentF1State;

        // Handle camera zoom with + and - keys
        if (keyState[SDL_SCANCODE_EQUALS])
        {
            mCamera->adjustZoom(deltaTime); // Zoom in
        }
        if (keyState[SDL_SCANCODE_MINUS])
        {
            mCamera->adjustZoom(-deltaTime); // Zoom out
        }

        // Update game objects
        mPlayer->update(deltaTime, *mTileMap);
        mTileMap->update(deltaTime);
        mCamera->update(mPlayer->getX(), mPlayer->getY());
    }

    void ZuulGame::render()
    {
        float zoom = mCamera->getZoom();
        float offsetX = mCamera->getOffsetX();
        float offsetY = mCamera->getOffsetY();

        // Render map layers
        mTileMap->render(getRenderer(), offsetX, offsetY, zoom);

        // Render player
        mPlayer->render(getRenderer(), offsetX, offsetY, zoom);

        // Render debug info if enabled
        if (mDebugRendering)
        {
            mTileMap->renderDebugCollisions(getRenderer(), offsetX, offsetY, zoom);
        }

        // Render UI (always on top, no offset or zoom)
        mUI->render(getRenderer(), 0, 0, 1.0f);
    }

} // namespace zuul