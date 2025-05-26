#include "engine/sdl_renderer.hpp"
#include <SDL3_image/SDL_image.h>
#include <iostream>

namespace zuul
{

    SDLTexture::SDLTexture(GLuint glTexture) 
        : mGLTexture(glTexture), mWidth(0), mHeight(0)
    {
        // Get texture dimensions
        glBindTexture(GL_TEXTURE_2D, mGLTexture);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &mWidth);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &mHeight);
    }

    SDLTexture::~SDLTexture()
    {
        if (mGLTexture != 0)
        {
            glDeleteTextures(1, &mGLTexture);
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
          mWindow(nullptr),
          mGLContext(nullptr),
          mVAO(0),
          mVBO(0),
          mEBO(0)
    {
    }

    SDLRenderer::~SDLRenderer()
    {
        cleanup();
    }

    void SDLRenderer::setupOpenGL()
    {
        // Set OpenGL attributes
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    }

    void SDLRenderer::createBuffers()
    {
        // Create and bind VAO
        glGenVertexArrays(1, &mVAO);
        glBindVertexArray(mVAO);

        // Create and bind VBO
        glGenBuffers(1, &mVBO);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        // Create and bind EBO
        glGenBuffers(1, &mEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);

        // Set up vertex attributes
        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // Unbind VAO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void SDLRenderer::destroyBuffers()
    {
        if (mEBO != 0) {
            glDeleteBuffers(1, &mEBO);
            mEBO = 0;
        }
        if (mVBO != 0) {
            glDeleteBuffers(1, &mVBO);
            mVBO = 0;
        }
        if (mVAO != 0) {
            glDeleteVertexArrays(1, &mVAO);
            mVAO = 0;
        }
    }

    bool SDLRenderer::initialize(int windowWidth, int windowHeight, const std::string &windowTitle)
    {
        // Initialize SDL
        if (SDL_Init(SDL_INIT_VIDEO) == false)
        {
            std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Set up OpenGL attributes
        setupOpenGL();

        // Create window with OpenGL support
        mWindow = SDL_CreateWindow(windowTitle.c_str(), windowWidth, windowHeight, SDL_WINDOW_OPENGL);
        if (!mWindow)
        {
            std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Create OpenGL context
        mGLContext = SDL_GL_CreateContext(mWindow);
        if (!mGLContext)
        {
            std::cerr << "OpenGL context could not be created! SDL_Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Initialize GLEW
        glewExperimental = GL_TRUE;
        GLenum err = glewInit();
        if (err != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
            return false;
        }

        // Print OpenGL version
        std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
        std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

        // Enable blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Set viewport
        glViewport(0, 0, windowWidth, windowHeight);
        mWindowWidth = windowWidth;
        mWindowHeight = windowHeight;

        // Create OpenGL buffers
        createBuffers();

        // Initialize SDL_ttf
        if (TTF_Init() == false)
        {
            std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << SDL_GetError() << std::endl;
            return false;
        }

        // Load font
        mFont = TTF_OpenFont("assets/fonts/OpenSans-Regular.ttf", 16);
        if (!mFont)
        {
            std::cerr << "Failed to load font! SDL_ttf Error: " << SDL_GetError() << std::endl;
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

        destroyBuffers();

        if (mGLContext)
        {
            SDL_GL_DestroyContext(mGLContext);
            mGLContext = nullptr;
        }

        if (mWindow)
        {
            SDL_DestroyWindow(mWindow);
            mWindow = nullptr;
        }

        TTF_Quit();
        SDL_Quit();
    }

    void SDLRenderer::clear()
    {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // Change to a visible color for testing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void SDLRenderer::present()
    {
        SDL_GL_SwapWindow(mWindow);
    }

    void SDLRenderer::useShader(const std::string& shaderName)
    {
        mCurrentShader = ShaderManager::getInstance().getShader(shaderName);
        if (mCurrentShader)
        {
            mCurrentShader->use();
        }
    }

    void SDLRenderer::disableShader()
    {
        mCurrentShader = nullptr;
        glUseProgram(0);
    }

    std::shared_ptr<Texture> SDLRenderer::loadTexture(const std::string &path)
    {
        SDL_Surface *surface = IMG_Load(path.c_str());
        if (!surface)
        {
            std::cerr << "Unable to load image " << path << "! SDL_image Error: " << SDL_GetError() << std::endl;
            return nullptr;
        }

        // Convert surface to OpenGL texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload texture data
        GLenum format = GL_RGBA;
        if (surface->format == SDL_PIXELFORMAT_RGB24)
        {
            format = GL_RGB;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Create a dummy SDL texture (we don't actually use it anymore)
        SDL_Texture *sdlTexture = nullptr;
        SDL_DestroySurface(surface);

        return std::make_shared<SDLTexture>(texture);
    }

    void SDLRenderer::renderTexture(std::shared_ptr<Texture> texture, int srcX, int srcY, int srcW, int srcH,
                                    int destX, int destY, int destW, int destH)
    {
        auto sdlTexture = std::dynamic_pointer_cast<SDLTexture>(texture);
        if (!sdlTexture)
        {
            std::cerr << "Failed to cast texture to SDLTexture" << std::endl;
            return;
        }

        // Calculate normalized texture coordinates
        float texLeft = static_cast<float>(srcX) / texture->getWidth();
        float texRight = static_cast<float>(srcX + srcW) / texture->getWidth();
        float texTop = static_cast<float>(srcY) / texture->getHeight();
        float texBottom = static_cast<float>(srcY + srcH) / texture->getHeight();

        // Calculate normalized device coordinates and clamp to [-1, 1]
        float ndcLeft = std::max(-1.0f, std::min(1.0f, (2.0f * destX / mWindowWidth) - 1.0f));
        float ndcRight = std::max(-1.0f, std::min(1.0f, (2.0f * (destX + destW) / mWindowWidth) - 1.0f));
        float ndcTop = std::max(-1.0f, std::min(1.0f, 1.0f - (2.0f * destY / mWindowHeight)));
        float ndcBottom = std::max(-1.0f, std::min(1.0f, 1.0f - (2.0f * (destY + destH) / mWindowHeight)));

        std::cout << "Rendering texture at: " << destX << "," << destY << " size: " << destW << "x" << destH << std::endl;
        std::cout << "NDC coordinates: " << ndcLeft << "," << ndcTop << " to " << ndcRight << "," << ndcBottom << std::endl;
        std::cout << "Texture coordinates: " << texLeft << "," << texTop << " to " << texRight << "," << texBottom << std::endl;

        // Vertex data
        float vertices[] = {
            // positions        // texture coords
            ndcLeft,  ndcTop,    0.0f, texLeft,  texTop,    // top left
            ndcRight, ndcTop,    0.0f, texRight, texTop,    // top right
            ndcRight, ndcBottom, 0.0f, texRight, texBottom, // bottom right
            ndcLeft,  ndcBottom, 0.0f, texLeft,  texBottom  // bottom left
        };

        // Element indices
        unsigned int indices[] = {
            0, 1, 2, // first triangle
            2, 3, 0  // second triangle
        };

        // Update VBO and EBO
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sdlTexture->getGLTexture());

        // Use shader if one is active
        if (mCurrentShader)
        {
            std::cout << "Using active shader" << std::endl;
            mCurrentShader->use();
            mCurrentShader->setUniform("texture1", 0); // Set texture unit
            mCurrentShader->setUniform("time", SDL_GetTicks() / 1000.0f);
        }
        else
        {
            std::cout << "No shader active, using default shader" << std::endl;
            // Use default shader if none is active
            static GLuint defaultShader = 0;
            if (defaultShader == 0) {
                // Create a simple default shader
                const char* vertexShaderSource = R"(
                    #version 330 core
                    layout (location = 0) in vec3 aPos;
                    layout (location = 1) in vec2 aTexCoord;
                    out vec2 TexCoord;
                    void main()
                    {
                        gl_Position = vec4(aPos, 1.0);
                        TexCoord = aTexCoord;
                    }
                )";
                const char* fragmentShaderSource = R"(
                    #version 330 core
                    in vec2 TexCoord;
                    out vec4 FragColor;
                    uniform sampler2D texture1;
                    void main()
                    {
                        vec4 texColor = texture(texture1, TexCoord);
                        if(texColor.a < 0.1)
                            discard;
                        FragColor = texColor;
                    }
                )";
                
                GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
                glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
                glCompileShader(vertexShader);
                
                GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
                glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
                glCompileShader(fragmentShader);
                
                defaultShader = glCreateProgram();
                glAttachShader(defaultShader, vertexShader);
                glAttachShader(defaultShader, fragmentShader);
                glLinkProgram(defaultShader);
                
                glDeleteShader(vertexShader);
                glDeleteShader(fragmentShader);
            }
            glUseProgram(defaultShader);
            glUniform1i(glGetUniformLocation(defaultShader, "texture1"), 0);
        }

        // Draw
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Check for OpenGL errors
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error in renderTexture: " << err << std::endl;
        }
    }

    void SDLRenderer::renderRect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        std::cout << "Rendering rect at: " << x << "," << y << " size: " << w << "x" << h << std::endl;

        // Calculate normalized device coordinates
        float ndcLeft = (2.0f * x / mWindowWidth) - 1.0f;
        float ndcRight = (2.0f * (x + w) / mWindowWidth) - 1.0f;
        float ndcTop = 1.0f - (2.0f * y / mWindowHeight);
        float ndcBottom = 1.0f - (2.0f * (y + h) / mWindowHeight);

        // Vertex data
        float vertices[] = {
            // positions        // color
            ndcLeft,  ndcTop,    0.0f, r/255.0f, g/255.0f, b/255.0f, a/255.0f,
            ndcRight, ndcTop,    0.0f, r/255.0f, g/255.0f, b/255.0f, a/255.0f,
            ndcRight, ndcBottom, 0.0f, r/255.0f, g/255.0f, b/255.0f, a/255.0f,
            ndcLeft,  ndcBottom, 0.0f, r/255.0f, g/255.0f, b/255.0f, a/255.0f
        };

        // Element indices
        unsigned int indices[] = {
            0, 1, 2, // first triangle
            2, 3, 0  // second triangle
        };

        // Update VBO and EBO
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

        // Draw
        glBindVertexArray(mVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Check for OpenGL errors
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR)
        {
            std::cerr << "OpenGL error in renderRect: " << err << std::endl;
        }
    }

    void SDLRenderer::renderText(const std::string &text, int x, int y, const Color &color)
    {
        if (!mFont)
        {
            return;
        }

        SDL_Color sdlColor = {color.r, color.g, color.b, color.a};
        SDL_Surface *surface = TTF_RenderText_Blended(mFont, text.c_str(), 0, sdlColor);
        if (!surface)
        {
            std::cerr << "Unable to render text surface! SDL_ttf Error: " << SDL_GetError() << std::endl;
            return;
        }

        // Create OpenGL texture from surface
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Render the texture
        renderTexture(std::make_shared<SDLTexture>(texture),
                     0, 0, surface->w, surface->h,
                     x, y, surface->w, surface->h);

        // Clean up
        glDeleteTextures(1, &texture);
        SDL_DestroySurface(surface);
    }

}