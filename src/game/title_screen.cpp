#include <game/title_screen.hpp>
#include <SDL2/SDL.h>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace zuul
{
    TitleScreen::TitleScreen()
        : mAnimationTimer(0.0f),
          mFrameDuration(0.1f), // 10 frames per second
          mCurrentFrame(0),
          mIsDone(false),
          mWindowWidth(0),
          mWindowHeight(0)
    {
    }

    bool TitleScreen::initialize(std::shared_ptr<Renderer> renderer)
    {
        // Get window size
        SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &mWindowWidth, &mWindowHeight);

        // Load all title screen frames
        std::string basePath = "assets/title_screen_";
        int frameNumber = 1;

        while (true)
        {
            std::string frameNumberStr = std::to_string(frameNumber);
            // Pad with zeros to make it 4 digits
            frameNumberStr = std::string(4 - frameNumberStr.length(), '0') + frameNumberStr;
            std::string framePath = basePath + frameNumberStr + ".png";

            // Try to load the texture
            auto texture = renderer->loadTexture(framePath);
            if (!texture)
            {
                if (frameNumber == 1)
                {
                    std::cerr << "Failed to load first title screen frame: " << framePath << std::endl;
                    return false;
                }
                break; // No more frames to load
            }

            mFrames.push_back(texture);
            frameNumber++;
        }

        std::cout << "Loaded " << mFrames.size() << " title screen frames" << std::endl;
        return !mFrames.empty();
    }

    void TitleScreen::update(float deltaTime)
    {
        // Check for any key press
        const Uint8 *keyState = SDL_GetKeyboardState(nullptr);
        int numKeys;
        SDL_PumpEvents();
        keyState = SDL_GetKeyboardState(&numKeys);

        for (int i = 0; i < numKeys; ++i)
        {
            if (keyState[i])
            {
                mIsDone = true;
                return;
            }
        }

        // Update animation
        mAnimationTimer += deltaTime;
        if (mAnimationTimer >= mFrameDuration)
        {
            mAnimationTimer -= mFrameDuration;
            mCurrentFrame = (mCurrentFrame + 1) % mFrames.size();
        }
    }

    void TitleScreen::render(std::shared_ptr<Renderer> renderer)
    {
        if (mCurrentFrame < mFrames.size())
        {
            auto texture = mFrames[mCurrentFrame];

            // Calculate scaling to fit the window while maintaining aspect ratio
            float scaleX = static_cast<float>(mWindowWidth) / texture->getWidth();
            float scaleY = static_cast<float>(mWindowHeight) / texture->getHeight();
            float scale = std::min(scaleX, scaleY);

            int destWidth = static_cast<int>(texture->getWidth() * scale);
            int destHeight = static_cast<int>(texture->getHeight() * scale);

            // Center the image
            int destX = (mWindowWidth - destWidth) / 2;
            int destY = (mWindowHeight - destHeight) / 2;

            // Render the current frame
            renderer->renderTexture(texture,
                                    0, 0, texture->getWidth(), texture->getHeight(),
                                    destX, destY, destWidth, destHeight);
        }
    }
}