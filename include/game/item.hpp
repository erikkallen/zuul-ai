#pragma once

#include <memory>
#include <engine/renderer.hpp>
#include <game/tileset_data.hpp>

namespace zuul
{
    class Item
    {
    public:
        Item(int tileId, float x, float y, std::shared_ptr<TilesetData> tilesetData, std::shared_ptr<Texture> texture);
        ~Item() = default;

        void update(float deltaTime);
        void render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom);

        bool isColliding(float x, float y, float width, float height) const;
        bool isCollected() const { return mCollected; }
        void collect() { mCollected = true; }

    private:
        int mTileId;
        float mX;
        float mY;
        int mWidth;
        int mHeight;
        bool mCollected;
        std::shared_ptr<TilesetData> mTilesetData;
        std::shared_ptr<Texture> mTexture;
    };

} // namespace zuul