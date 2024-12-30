#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

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

    class TilesetData
    {
    public:
        bool loadFromFile(const ::std::string &filepath);
        void update(float deltaTime);

        // Animation methods
        bool hasAnimation(int tileId) const;
        int getCurrentTileId(int baseTileId) const;
        const TileAnimation *getAnimation(int tileId) const;

        // Collision methods
        bool hasCollisionBox(int tileId) const;
        const CollisionBox *getCollisionBox(int tileId) const;
        bool isSolid(int tileId) const;

    private:
        ::std::map<int, TileAnimation> mAnimations;
        ::std::map<int, CollisionBox> mCollisionBoxes;
        ::std::map<int, bool> mSolidTiles;
    };

} // namespace zuul