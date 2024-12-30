#pragma once

#include <engine/renderer.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace zuul
{
    struct TileAnimation
    {
        std::vector<int> tileIds;
        std::vector<float> durations;
        int currentFrame;
        float timer;
    };

    struct MapLayer
    {
        std::string name;
        std::vector<int> tileData;
        bool visible;
        bool collision;
    };

    struct CollisionBox
    {
        float x;
        float y;
        float width;
        float height;
    };

    class TileMap
    {
    public:
        TileMap();
        bool loadFromFile(const std::string &mapPath, const std::string &tilesetPath, std::shared_ptr<Renderer> renderer);
        void update(float deltaTime);
        void render(std::shared_ptr<Renderer> renderer, float offsetX = 0, float offsetY = 0);
        void renderDebugCollisions(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY);

        // Collision detection
        bool isSolid(float x, float y) const;
        bool checkCollision(float x, float y, float width, float height) const;

        // Debug options
        void setDebugRendering(bool enabled) { mDebugRendering = enabled; }
        bool getDebugRendering() const { return mDebugRendering; }

        // Getters for map dimensions
        int getWidth() const { return mWidth; }
        int getHeight() const { return mHeight; }
        int getTileWidth() const { return mTileWidth; }
        int getTileHeight() const { return mTileHeight; }

    private:
        int getCurrentTileId(int tileId) const;
        void renderLayer(const MapLayer &layer, std::shared_ptr<Renderer> renderer, float offsetX, float offsetY);
        std::pair<int, int> worldToTile(float x, float y) const;

        std::shared_ptr<Texture> mTileset;
        std::vector<MapLayer> mLayers;
        std::unordered_map<int, TileAnimation> mAnimatedTiles;
        std::unordered_set<int> mSolidTiles;                   // Tiles marked as solid with property
        std::unordered_map<int, CollisionBox> mCollisionBoxes; // Tiles with collision boxes
        int mWidth;
        int mHeight;
        int mTileWidth;
        int mTileHeight;
        int mTilesetColumns;
        bool mDebugRendering;
    };
} // namespace zuul