#include <game/item.hpp>
#include <cmath>

namespace zuul
{
    Item::Item(int tileId, float x, float y, std::shared_ptr<TilesetData> tilesetData, std::shared_ptr<Texture> texture)
        : mTileId(tileId),
          mX(x),
          mY(y),
          mWidth(tilesetData->getTilesetInfo().tileWidth),
          mHeight(tilesetData->getTilesetInfo().tileHeight),
          mCollected(false),
          mTilesetData(tilesetData),
          mTexture(texture)
    {
    }

    void Item::update(float deltaTime)
    {
        // No need to update TilesetData here as it's handled by TileMap
    }

    void Item::render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom)
    {
        if (!mCollected && mTexture)
        {
            // Calculate screen position with zoom
            float screenX = std::floor((mX - offsetX) * zoom);
            float screenY = std::floor((mY - offsetY) * zoom);
            int destW = static_cast<int>(std::ceil(mWidth * zoom));
            int destH = static_cast<int>(std::ceil(mHeight * zoom));

            // Get current animation frame if tile is animated
            int currentTileId = mTilesetData->getCurrentTileId(mTileId);

            // Calculate source rectangle in tileset
            const auto &tilesetInfo = mTilesetData->getTilesetInfo();
            int srcX = (currentTileId % tilesetInfo.columns) * mWidth;
            int srcY = (currentTileId / tilesetInfo.columns) * mHeight;

            // Render the item
            renderer->renderTexture(mTexture,
                                    srcX, srcY, mWidth, mHeight,
                                    static_cast<int>(screenX),
                                    static_cast<int>(screenY),
                                    destW, destH);
        }
    }

    bool Item::isColliding(float x, float y, float width, float height) const
    {
        if (mCollected)
        {
            return false;
        }

        // Simple AABB collision check
        return (x < mX + mWidth && x + width > mX &&
                y < mY + mHeight && y + height > mY);
    }

    void Item::collect()
    {
        if (!mCollected)
        {
            mCollected = true;
            if (mCollectCallback)
            {
                mCollectCallback(mTileId);
            }
        }
    }

} // namespace zuul