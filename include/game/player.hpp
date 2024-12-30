#pragma once

#include <SDL2/SDL.h>
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

        bool initialize(std::shared_ptr<Renderer> renderer);
        void update(float deltaTime, const TileMap &tileMap);
        void render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom);

        void setPosition(float x, float y)
        {
            mX = x;
            mY = y;
        }
        float getX() const { return mX; }
        float getY() const { return mY; }
        void setDebugRendering(bool enabled) { mDebugRendering = enabled; }

    private:
        int getBaseFrame() const;
        bool loadCollisionData();

        Direction mDirection;
        std::shared_ptr<TilesetData> mTilesetData;
        std::shared_ptr<Texture> mTexture;
        float mX;
        float mY;
        float mSpeed;
        int mWidth;
        int mHeight;
        int mTilesetColumns;
        bool mDebugRendering;

        // Collision box
        float mCollisionBoxOffsetX;
        float mCollisionBoxOffsetY;
        float mCollisionBoxWidth;
        float mCollisionBoxHeight;
    };

} // namespace zuul