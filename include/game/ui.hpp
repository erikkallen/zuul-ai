#pragma once

#include <memory>
#include <string>
#include <map>
#include <engine/renderer.hpp>
#include <game/tileset_data.hpp>

namespace zuul
{
    class UI
    {
    public:
        UI();
        ~UI() = default;

        bool initialize(std::shared_ptr<Renderer> renderer);
        void render(std::shared_ptr<Renderer> renderer);

        // Item collection
        void addCollectedItem(int itemId);
        int getCollectedItemCount(int itemId) const;

    private:
        std::shared_ptr<Texture> mItemTexture;
        std::shared_ptr<TilesetData> mTilesetData;
        std::map<int, int> mCollectedItems; // itemId -> count
        int mTileWidth;
        int mTileHeight;
    };

} // namespace zuul