#pragma once

#include <engine/renderer.hpp>
#include <game/tileset_data.hpp>
#include <memory>
#include <vector>
#include <string>

namespace zuul
{

    struct MapLayer
    {
        ::std::string name;
        ::std::vector<int> tileData;
        bool visible;
    };

    class TileMap
    {
    public:
        TileMap();
        ~TileMap() = default;

        bool loadFromFile(const ::std::string &mapFile, const ::std::string &tilesetFile, ::std::shared_ptr<Renderer> renderer);
        void update(float deltaTime);
        void render(::std::shared_ptr<Renderer> renderer, float offsetX = 0, float offsetY = 0);
        void renderDebugCollisions(::std::shared_ptr<Renderer> renderer, float offsetX = 0, float offsetY = 0);

        // Collision detection
        bool checkCollision(float x, float y, float width, float height) const;

        // Debug options
        void setDebugRendering(bool enabled) { mDebugRendering = enabled; }
        bool getDebugRendering() const { return mDebugRendering; }

        // Getters
        int getWidth() const { return mWidth; }
        int getHeight() const { return mHeight; }
        int getTileWidth() const { return mTileWidth; }
        int getTileHeight() const { return mTileHeight; }

    private:
        void renderLayer(const MapLayer &layer, ::std::shared_ptr<Renderer> renderer, float offsetX, float offsetY);
        ::std::pair<int, int> worldToTile(float x, float y) const;

        ::std::shared_ptr<Texture> mTileset;
        ::std::unique_ptr<TilesetData> mTilesetData;
        ::std::vector<MapLayer> mLayers;

        int mWidth;
        int mHeight;
        int mTileWidth;
        int mTileHeight;
        int mTilesetColumns;
        bool mDebugRendering;
    };

} // namespace zuul