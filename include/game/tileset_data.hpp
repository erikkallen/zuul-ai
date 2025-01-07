#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <engine/renderer.hpp>

namespace zuul
{

    struct AnimationFrame
    {
        int tileId;
        float duration;
    };

    struct TileAnimation
    {
        ::std::vector<AnimationFrame> frames;
        int currentFrameIndex = 0;
        float timer = 0.0f;
    };

    struct CollisionBox
    {
        float x;
        float y;
        float width;
        float height;
    };

    struct TilesetInfo
    {
        int columns;
        int tileWidth;
        int tileHeight;
        ::std::string imagePath;
    };

    class TilesetData
    {
    public:
        bool loadFromFile(const ::std::string &filepath, std::shared_ptr<Renderer> renderer);
        void update(float deltaTime);

        // Animation methods
        bool hasAnimation(int tileId) const;
        int getCurrentTileId(int baseTileId) const;
        const TileAnimation *getAnimation(int tileId) const;

        // Collision methods
        bool hasCollisionBox(int tileId) const;
        const CollisionBox *getCollisionBox(int tileId) const;
        bool isSolid(int tileId) const;

        // Tileset info
        const TilesetInfo &getTilesetInfo() const { return mTilesetInfo; }
        std::shared_ptr<Texture> getTexture() const { return mTexture; }

        // Render methods
        void renderTile(std::shared_ptr<Renderer> renderer, int tileId, float x, float y, float zoom = 1.0f) const;

    private:
        ::std::map<int, TileAnimation> mAnimations;
        ::std::map<int, CollisionBox> mCollisionBoxes;
        ::std::map<int, bool> mSolidTiles;
        TilesetInfo mTilesetInfo;
        std::shared_ptr<Texture> mTexture;
    };

} // namespace zuul