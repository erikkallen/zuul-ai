#pragma once

namespace zuul
{
    class Texture
    {
    public:
        virtual ~Texture() = default;
        virtual int getWidth() const = 0;
        virtual int getHeight() const = 0;
    };
} // namespace zuul