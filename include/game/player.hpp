#pragma once

#include <engine/renderer.hpp>
#include <memory>
#include <string>
#include <vector>

namespace zuul
{

    enum class Direction
    {
        Down = 0,  // Tiles 0-2
        Left = 1,  // Tiles 3-5
        Right = 2, // Tiles 6-8
        Up = 3     // Tiles 9-11
    };

    struct AnimationFrame
    {
        int tileId;
        float duration;
    };

    struct AnimationData
    {
        std::vector<AnimationFrame> frames;
        int currentFrameIndex;
    };

    class Player
    {
    public:
        Player();
        ~Player() = default;

        bool initialize(::std::shared_ptr<Renderer> renderer);
        void update(float deltaTime);
        void render(::std::shared_ptr<Renderer> renderer);

        // Getters
        float getX() const { return mX; }
        float getY() const { return mY; }
        float getSpeed() const { return mSpeed; }
        Direction getDirection() const { return mDirection; }

        // Setters
        void setPosition(float x, float y)
        {
            mX = x;
            mY = y;
        }
        void setSpeed(float speed) { mSpeed = speed; }

    private:
        bool loadTilesetData();
        void updateAnimation(float deltaTime, bool isMoving);
        int getCurrentTileId() const;

        ::std::shared_ptr<Texture> mTexture;
        float mX;
        float mY;
        float mSpeed;
        int mWidth;
        int mHeight;
        float mAnimationTimer;
        Direction mDirection;
        ::std::vector<AnimationData> mDirectionAnimations;
    };

} // namespace zuul