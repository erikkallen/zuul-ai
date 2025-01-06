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

    SDLRenderer::SDLRenderer()
        : Renderer(),
          mWindow(nullptr)
    {
    }

    SDLRenderer::~SDLRenderer()
    {
        cleanup();
    }

    bool SDLRenderer::initialize(int windowWidth, int windowHeight, const std::string &windowTitle)
    {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Initialize SDL_image
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags))
        {
            std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
            return false;
        }

        // Initialize SDL_ttf
        if (TTF_Init() == -1)
        {
            std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
            return false;
        }

        // Create window
        mWindow = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                   windowWidth, windowHeight, SDL_WINDOW_SHOWN);
        if (!mWindow)
        {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create renderer
        mRenderer = SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!mRenderer)
        {
            std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Load font
        mFont = TTF_OpenFont("assets/fonts/OpenSans-Regular.ttf", 16);
        if (!mFont)
        {
            std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
            return false;
        }

        return true;
    }

    void SDLRenderer::cleanup()
    {
        if (mFont)
        {
            TTF_CloseFont(mFont);
            mFont = nullptr;
        }

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

        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    void SDLRenderer::clear()
    {
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
        SDL_RenderClear(mRenderer);
    }

    void SDLRenderer::present()
    {
        SDL_RenderPresent(mRenderer);
    }

    std::shared_ptr<Texture> SDLRenderer::loadTexture(const std::string &path)
    {
        SDL_Surface *surface = IMG_Load(path.c_str());
        if (!surface)
        {
            std::cerr << "Unable to load image " << path << "! SDL_image Error: " << IMG_GetError() << std::endl;
            return nullptr;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
        SDL_FreeSurface(surface);

        if (!texture)
        {
            std::cerr << "Unable to create texture from " << path << "! SDL Error: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        return std::make_shared<SDLTexture>(texture);
    }

    void SDLRenderer::renderTexture(std::shared_ptr<Texture> texture, int srcX, int srcY, int srcW, int srcH,
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
        SDL_SetRenderDrawColor(mRenderer, r, g, b, a);
        SDL_Rect rect = {x, y, w, h};
        SDL_RenderDrawRect(mRenderer, &rect);
    }

    void SDLRenderer::renderText(const std::string &text, int x, int y, const Color &color)
    {
        if (!mFont)
        {
            return;
        }

        SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
        SDL_Surface *surface = TTF_RenderText_Blended(mFont, text.c_str(), sdlColor);
        if (!surface)
        {
            std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
            return;
        }

        SDL_Texture *texture = SDL_CreateTextureFromSurface(mRenderer, surface);
        if (!texture)
        {
            std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(surface);
            return;
        }

        SDL_Rect destRect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(mRenderer, texture, nullptr, &destRect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

}