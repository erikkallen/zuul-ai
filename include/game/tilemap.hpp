#pragma once

#include <SDL2/SDL.h>
#include <engine/renderer.hpp>
#include <game/tileset_data.hpp>
#include <memory>
#include <vector>
#include <string>
#include <game/item.hpp>
#include <game/ui.hpp>

namespace zuul
{

    struct MapLayer
    {
        std::string name;
        std::vector<unsigned int> tileData;
        bool visible;
    };

    class TileMap
    {
    public:
        TileMap();
        ~TileMap() = default;

        bool loadFromFile(const std::string &filepath, std::shared_ptr<Renderer> renderer);
        void update(float deltaTime);
        void render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom);
        void renderDebugCollisions(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom);

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

        // Item handling
        void checkItemCollisions(float x, float y, float width, float height) const;
        void renderItems(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom);

        // UI
        void renderUI(std::shared_ptr<Renderer> renderer);

    private:
        std::pair<int, int> worldToTile(float x, float y) const;

        std::shared_ptr<Texture> mTileset;
        std::shared_ptr<TilesetData> mTilesetData;
        std::vector<MapLayer> mLayers;

        int mWidth;
        int mHeight;
        int mTileWidth;
        int mTileHeight;
        int mTilesetColumns;
        int mWindowWidth;
        int mWindowHeight;
        bool mDebugRendering;

        mutable std::vector<Item> mItems;
        UI mUI;
    };

} // namespace zuul