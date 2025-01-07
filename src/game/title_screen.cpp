#include <game/title_screen.hpp>
#include <SDL2/SDL.h>
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace zuul
{
    TitleScreen::TitleScreen()
        : mAnimationTimer(0.0f),
          mFrameDuration(0.2f), // 5 frames per second
          mBlinkTimer(0.0f),
          mBlinkDuration(0.5f), // Blink every half second
          mShowText(true),
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

        // Load background
        mBackground = renderer->loadTexture("assets/title_screen_background.png");
        if (!mBackground)
        {
            std::cerr << "Failed to load title background: assets/title_screen_background.png" << std::endl;
            return false;
        }

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

        // Update text blinking
        mBlinkTimer += deltaTime;
        if (mBlinkTimer >= mBlinkDuration)
        {
            mBlinkTimer -= mBlinkDuration;
            mShowText = !mShowText;
        }
    }

    void TitleScreen::render(std::shared_ptr<Renderer> renderer)
    {
        // Set background color (hex 69bd2f = RGB 105, 189, 47)
        renderer->clear();

        // Render tiled background
        if (mBackground)
        {
            int bgWidth = mBackground->getWidth();
            int bgHeight = mBackground->getHeight();

            // Calculate how many tiles we need in each direction
            int tilesX = (mWindowWidth + bgWidth - 1) / bgWidth;
            int tilesY = (mWindowHeight + bgHeight - 1) / bgHeight;

            // Render the background tiles
            for (int y = 0; y <= tilesY; y++)
            {
                for (int x = 0; x <= tilesX; x++)
                {
                    int destX = x * bgWidth;
                    int destY = y * bgHeight;

                    renderer->renderTexture(mBackground,
                                            0, 0, bgWidth, bgHeight,
                                            destX, destY, bgWidth, bgHeight);
                }
            }
        }

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

        // Render blinking text in color #211f34
        if (mShowText)
        {
            renderer->renderText("Press any key to start", mWindowWidth / 2 - 100, mWindowHeight - 100, {33, 31, 52, 255});
        }
    }
}