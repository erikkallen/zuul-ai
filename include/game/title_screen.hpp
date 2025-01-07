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
        std::shared_ptr<Texture> mBackground;
        std::vector<std::shared_ptr<Texture>> mFrames;
        float mAnimationTimer;
        float mFrameDuration;
        float mBlinkTimer;
        float mBlinkDuration;
        bool mShowText;
        size_t mCurrentFrame;
        bool mIsDone;
        int mWindowWidth;
        int mWindowHeight;
    };
}