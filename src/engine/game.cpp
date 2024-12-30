#include "engine/game.hpp"
#include "engine/sdl_renderer.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <memory>
#include <iostream>

namespace zuul
{

    Game::Game() : mIsRunning(false) {}

    bool Game::initialize(int windowWidth, int windowHeight, const ::std::string &windowTitle)
    {
        mRenderer = ::std::make_shared<SDLRenderer>();
        if (!mRenderer->initialize(windowWidth, windowHeight, windowTitle))
        {
            return false;
        }

        mIsRunning = true;
        return true;
    }

    void Game::run()
    {
        uint32_t previousTime = SDL_GetTicks();
        float lag = 0.0f;

        while (mIsRunning)
        {
            uint32_t currentTime = SDL_GetTicks();
            float deltaTime = (currentTime - previousTime) / 1000.0f;
            previousTime = currentTime;
            lag += deltaTime;

            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT)
                {
                    mIsRunning = false;
                }
            }

            // Update game logic at fixed time step
            while (lag >= FRAME_TIME)
            {
                update(FRAME_TIME);
                lag -= FRAME_TIME;
            }

            // Render at whatever rate we can
            mRenderer->clear();
            render();
            mRenderer->present();
        }
    }

    void Game::stop()
    {
        mIsRunning = false;
    }
}