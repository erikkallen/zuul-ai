#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <engine/renderer.hpp>
#include <memory>
#include <string>

namespace zuul
{
    class SDLTexture : public Texture
    {
    public:
        explicit SDLTexture(SDL_Texture *texture);
        ~SDLTexture();

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

        bool initialize(int windowWidth, int windowHeight, const std::string &windowTitle);
        void cleanup();

        void clear() override;
        void present() override;

        std::shared_ptr<Texture> loadTexture(const std::string &path) override;
        void renderTexture(std::shared_ptr<Texture> texture,
                           int srcX, int srcY, int srcW, int srcH,
                           int destX, int destY, int destW, int destH) override;

        void renderRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void renderText(const std::string &text, int x, int y, const Color &color) override;

    private:
        SDL_Window *mWindow;
    };

} // namespace zuul