#include <game/ui.hpp>
#include <string>
#include <sstream>

namespace zuul
{
    UI::UI()
        : mTileWidth(32),
          mTileHeight(32)
    {
    }

    bool UI::initialize(std::shared_ptr<Renderer> renderer)
    {
        mItemTexture = renderer->loadTexture("assets/map_tiles.png");
        if (!mItemTexture)
        {
            return false;
        }

        mTilesetData = std::make_shared<TilesetData>();
        if (!mTilesetData->loadFromFile("assets/map_tiles.tsj"))
        {
            return false;
        }

        return true;
    }

    void UI::render(std::shared_ptr<Renderer> renderer)
    {
        // Render collected items in the top-left corner
        int x = 10;
        int y = 10;
        const int spacing = 5;
        const float scale = 1.0f;

        for (const auto &[itemId, count] : mCollectedItems)
        {
            // Render item icon
            const auto &tilesetInfo = mTilesetData->getTilesetInfo();
            int srcX = (itemId % tilesetInfo.columns) * mTileWidth;
            int srcY = (itemId / tilesetInfo.columns) * mTileHeight;

            renderer->renderTexture(mItemTexture,
                                    srcX, srcY, mTileWidth, mTileHeight,
                                    x, y,
                                    static_cast<int>(mTileWidth * scale),
                                    static_cast<int>(mTileHeight * scale));

            // Render count
            std::stringstream ss;
            ss << "x" << count;
            renderer->renderText(ss.str(), x + mTileWidth * scale + spacing, y + mTileHeight / 2 - 8,
                                 {255, 255, 255, 255}); // White text

            // Move to next position
            x += static_cast<int>(mTileWidth * scale + 50); // Add space for the count text
        }
    }

    void UI::addCollectedItem(int itemId)
    {
        mCollectedItems[itemId]++;
    }

    int UI::getCollectedItemCount(int itemId) const
    {
        auto it = mCollectedItems.find(itemId);
        return it != mCollectedItems.end() ? it->second : 0;
    }

} // namespace zuul