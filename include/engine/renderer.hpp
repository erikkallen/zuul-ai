#pragma once

#include <memory>
#include <string>

namespace zuul
{

    class Texture
    {
    public:
        virtual ~Texture() = default;
        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;
    };

    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual bool initialize(int windowWidth, int windowHeight, const ::std::string &windowTitle) = 0;
        virtual void clear() = 0;
        virtual void present() = 0;
        virtual void cleanup() = 0;

        virtual ::std::shared_ptr<Texture> loadTexture(const ::std::string &path) = 0;
        virtual void renderTexture(const ::std::shared_ptr<Texture> &texture, int srcX, int srcY, int srcW, int srcH,
                                   int destX, int destY, int destW, int destH) = 0;
    };
} // namespace zuul