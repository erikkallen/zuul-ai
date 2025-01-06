#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <memory>
#include <string>
#include "engine/texture.hpp"

namespace zuul
{
    struct Color
    {
        uint8_t r, g, b, a;
    };

    class Renderer
    {
    public:
        Renderer();
        virtual ~Renderer();

        virtual bool initialize(int windowWidth, int windowHeight, const std::string &windowTitle) = 0;
        virtual void clear() = 0;
        virtual void present() = 0;

        virtual std::shared_ptr<Texture> loadTexture(const std::string &path) = 0;
        virtual void renderTexture(std::shared_ptr<Texture> texture,
                                   int srcX, int srcY, int srcW, int srcH,
                                   int destX, int destY, int destW, int destH) = 0;

        virtual void renderRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) = 0;
        virtual void renderText(const std::string &text, int x, int y, const Color &color) = 0;

    protected:
        SDL_Renderer *mRenderer;
        TTF_Font *mFont;
    };

} // namespace zuul