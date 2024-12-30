#pragma once

#include "renderer.hpp"
#include <SDL2/SDL.h>
#include <string>

namespace zuul
{

    class SDLTexture : public Texture
    {
    public:
        SDLTexture(SDL_Texture *texture);
        ~SDLTexture() override;

        int getWidth() const override;
        int getHeight() const override;

        SDL_Texture *getSDLTexture() const { return mTexture; }

    private:
        SDL_Texture *mTexture;
        int mWidth;
        int mHeight;
    };

    class SDLRenderer : public Renderer
    {
    public:
        SDLRenderer();
        ~SDLRenderer() override;

        bool initialize(int windowWidth, int windowHeight, const ::std::string &windowTitle) override;
        void clear() override;
        void present() override;
        void cleanup() override;

        ::std::shared_ptr<Texture> loadTexture(const ::std::string &path) override;
        void renderTexture(const ::std::shared_ptr<Texture> &texture, int srcX, int srcY, int srcW, int srcH,
                           int destX, int destY, int destW, int destH) override;

    private:
        SDL_Window *mWindow;
        SDL_Renderer *mRenderer;
    };
} // namespace zuul