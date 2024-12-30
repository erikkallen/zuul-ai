#include "engine/sdl_renderer.hpp"
#include <SDL2/SDL_image.h>
#include <iostream>

namespace zuul
{

    SDLTexture::SDLTexture(SDL_Texture *texture) : mTexture(texture), mWidth(0), mHeight(0)
    {
        if (mTexture)
        {
            SDL_QueryTexture(mTexture, nullptr, nullptr, &mWidth, &mHeight);
        }
    }

    SDLTexture::~SDLTexture()
    {
        if (mTexture)
        {
            SDL_DestroyTexture(mTexture);
        }
    }

    int SDLTexture::getWidth() const
    {
        return mWidth;
    }

    int SDLTexture::getHeight() const
    {
        return mHeight;
    }

    SDLRenderer::SDLRenderer() : mWindow(nullptr), mRenderer(nullptr) {}

    SDLRenderer::~SDLRenderer()
    {
        cleanup();
    }

    bool SDLRenderer::initialize(int windowWidth, int windowHeight, const std::string &windowTitle)
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        {
            std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
            return false;
        }

        mWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   windowWidth, windowHeight, SDL_WINDOW_SHOWN);
        if (!mWindow)
        {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!mRenderer)
        {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        SDL_SetRenderDrawColor(mRenderer, 0x00, 0x00, 0x00, 0xFF);
        return true;
    }

    void SDLRenderer::clear()
    {
        SDL_RenderClear(mRenderer);
    }

    void SDLRenderer::present()
    {
        SDL_RenderPresent(mRenderer);
    }

    void SDLRenderer::cleanup()
    {
        if (mRenderer)
        {
            SDL_DestroyRenderer(mRenderer);
            mRenderer = nullptr;
        }
        if (mWindow)
        {
            SDL_DestroyWindow(mWindow);
            mWindow = nullptr;
        }
        IMG_Quit();
        SDL_Quit();
    }

    std::shared_ptr<Texture> SDLRenderer::loadTexture(const std::string &path)
    {
        SDL_Surface *loadedSurface = IMG_Load(path.c_str());
        if (!loadedSurface)
        {
            std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, loadedSurface);
        SDL_FreeSurface(loadedSurface);

        if (!texture)
        {
            std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        return std::make_shared<SDLTexture>(texture);
    }

    void SDLRenderer::renderTexture(const std::shared_ptr<Texture> &texture, int srcX, int srcY, int srcW, int srcH,
                                    int destX, int destY, int destW, int destH)
    {
        auto sdlTexture = std::dynamic_pointer_cast<SDLTexture>(texture);
        if (!sdlTexture)
        {
            return;
        }

        SDL_Rect srcRect = {srcX, srcY, srcW, srcH};
        SDL_Rect destRect = {destX, destY, destW, destH};
        SDL_RenderCopy(mRenderer, sdlTexture->getSDLTexture(), &srcRect, &destRect);
    }

    void SDLRenderer::renderRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        SDL_SetRenderDrawBlendMode(mRenderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(mRenderer, r, g, b, a);
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderDrawRect(mRenderer, &rect);
    }
}