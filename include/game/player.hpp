#pragma once

#include <engine/renderer.hpp>
#include <game/tileset_data.hpp>
#include <memory>

namespace zuul
{

    class TileMap; // Forward declaration

    enum class Direction
    {
        Down = 0,
        Up = 3,
        Left = 6,
        Right = 9
    };

    class Player
    {
    public:
        Player();
        ~Player() = default;

        bool initialize(::std::shared_ptr<Renderer> renderer);
        void update(float deltaTime, const TileMap &tileMap);
        void render(::std::shared_ptr<Renderer> renderer);
        void renderDebug(::std::shared_ptr<Renderer> renderer);

        // Getters
        float getX() const { return mX; }
        float getY() const { return mY; }
        float getSpeed() const { return mSpeed; }
        float getCollisionX() const { return mX + mCollisionBoxOffsetX; }
        float getCollisionY() const { return mY + mCollisionBoxOffsetY; }
        float getCollisionWidth() const { return mCollisionBoxWidth; }
        float getCollisionHeight() const { return mCollisionBoxHeight; }

        // Setters
        void setPosition(float x, float y)
        {
            mX = x;
            mY = y;
        }
        void setSpeed(float speed) { mSpeed = speed; }

    private:
        ::std::shared_ptr<Texture> mTexture;
        ::std::unique_ptr<TilesetData> mTilesetData;
        float mX;
        float mY;
        float mSpeed;
        int mWidth;
        int mHeight;
        Direction mDirection;

        // Collision box
        float mCollisionBoxOffsetX;
        float mCollisionBoxOffsetY;
        float mCollisionBoxWidth;
        float mCollisionBoxHeight;
    };

} // namespace zuul