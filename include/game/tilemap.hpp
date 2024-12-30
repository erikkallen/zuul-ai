#pragma once

#include <engine/renderer.hpp>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

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
    };

    class TileMap
    {
    public:
        TileMap();
        bool loadFromFile(const ::std::string &mapPath, const ::std::string &tilesetPath, ::std::shared_ptr<Renderer> renderer);
        void update(float deltaTime);
        void render(::std::shared_ptr<Renderer> renderer, float offsetX = 0, float offsetY = 0);

        // Getters for map dimensions
        int getWidth() const { return mWidth; }
        int getHeight() const { return mHeight; }
        int getTileWidth() const { return mTileWidth; }
        int getTileHeight() const { return mTileHeight; }

    private:
        int getCurrentTileId(int tileId) const;
        void renderLayer(const MapLayer &layer, ::std::shared_ptr<Renderer> renderer, float offsetX, float offsetY);

        ::std::shared_ptr<Texture> mTileset;
        ::std::vector<MapLayer> mLayers;
        ::std::unordered_map<int, TileAnimation> mAnimatedTiles;
        int mWidth;
        int mHeight;
        int mTileWidth;
        int mTileHeight;
        int mTilesetColumns;
    };
} // namespace zuul