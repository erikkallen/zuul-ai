#pragma once

#include "renderer.hpp"
#include <memory>
#include <string>

namespace zuul
{

    class Game
    {
    public:
        Game();
        virtual ~Game() = default;

        virtual bool initialize(int windowWidth, int windowHeight, const ::std::string &windowTitle);
        void run();
        void stop();

    protected:
        virtual void update(float deltaTime) = 0;
        virtual void render() = 0;

        ::std::shared_ptr<Renderer> getRenderer() { return mRenderer; }

    private:
        ::std::shared_ptr<Renderer> mRenderer;
        bool mIsRunning;
        const int TARGET_FPS = 60;
        const float FRAME_TIME = 1.0f / TARGET_FPS;
    };

} // namespace zuul