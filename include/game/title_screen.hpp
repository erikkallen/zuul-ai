#pragma once

#include <memory>
#include <vector>
#include <string>
#include <engine/renderer.hpp>

namespace zuul
{
    class TitleScreen
    {
    public:
        TitleScreen();
        ~TitleScreen() = default;

        bool initialize(std::shared_ptr<Renderer> renderer);
        void update(float deltaTime);
        void render(std::shared_ptr<Renderer> renderer);
        bool isDone() const { return mIsDone; }

    private:
        std::vector<std::shared_ptr<Texture>> mFrames;
        float mAnimationTimer;
        float mFrameDuration;
        size_t mCurrentFrame;
        bool mIsDone;
        int mWindowWidth;
        int mWindowHeight;
    };
}