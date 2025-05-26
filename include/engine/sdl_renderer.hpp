#pragma once

#include "engine/renderer.hpp"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <SDL3_image/SDL_image.h>
#include <GL/glew.h>
#include <memory>
#include <string>
#include "engine/shader.hpp"

namespace zuul
{
    class SDLTexture : public Texture
    {
    public:
        SDLTexture(GLuint glTexture);
        ~SDLTexture();

        int getWidth() const override;
        int getHeight() const override;
        GLuint getGLTexture() const { return mGLTexture; }

    private:
        GLuint mGLTexture;
        int mWidth;
        int mHeight;
    };

    class SDLRenderer : public Renderer
    {
    public:
        SDLRenderer();
        ~SDLRenderer();

        bool initialize(int windowWidth, int windowHeight, const std::string &windowTitle) override;
        void cleanup() override;

        void clear() override;
        void present() override;

        std::shared_ptr<Texture> loadTexture(const std::string &path) override;
        void renderTexture(std::shared_ptr<Texture> texture, int srcX, int srcY, int srcW, int srcH,
                          int destX, int destY, int destW, int destH) override;
        void renderTextureWithShader(std::shared_ptr<Texture> texture, int srcX, int srcY, int srcW, int srcH,
                                    int destX, int destY, int destW, int destH, const std::string& shaderName);
        void renderRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a) override;
        void renderText(const std::string &text, int x, int y, const Color &color) override;

        void useShader(const std::string& shaderName);
        void disableShader();

    private:
        void setupOpenGL();
        void createBuffers();
        void destroyBuffers();

        SDL_Window *mWindow;
        SDL_GLContext mGLContext;
        TTF_Font *mFont;
        int mWindowWidth;
        int mWindowHeight;

        // OpenGL objects
        GLuint mVAO;
        GLuint mVBO;
        GLuint mEBO;
        std::shared_ptr<Shader> mCurrentShader;
    };
}