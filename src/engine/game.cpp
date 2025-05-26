#include "engine/game.hpp"
#include "engine/sdl_renderer.hpp"
#include "engine/shader.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>
#include <memory>
#include <iostream>

namespace zuul
{

    Game::Game() : mIsRunning(false) {}

    bool Game::initialize(int windowWidth, int windowHeight, const ::std::string &windowTitle)
    {
        // Create and initialize renderer first
        mRenderer = ::std::make_shared<SDLRenderer>();
        if (!mRenderer->initialize(windowWidth, windowHeight, windowTitle))
        {
            return false;
        }

        // Initialize shader manager
        if (!ShaderManager::getInstance().initialize())
        {
            std::cerr << "Failed to initialize shader manager" << std::endl;
            return false;
        }

        // Load shaders
        auto invertShader = ShaderManager::getInstance().loadShader(
            "invert",
            "assets/shaders/invert.vert",
            "assets/shaders/invert.frag"
        );
        if (!invertShader)
        {
            std::cerr << "Failed to load invert shader" << std::endl;
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
                if (event.type == SDL_EVENT_QUIT)
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