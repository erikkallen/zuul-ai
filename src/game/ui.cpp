#include <game/ui.hpp>
#include <string>
#include <sstream>
#include <SDL2/SDL.h>
#include <iostream>
namespace zuul
{
    UI::UI()
        : TileMap()
    {
    }

    bool UI::initialize(std::shared_ptr<Renderer> renderer)
    {
        if (!loadFromFile("assets/ui.tmj", renderer))
        {
            return false;
        }

        // Get window size
        int windowWidth, windowHeight;
        SDL_GetWindowSize(SDL_GL_GetCurrentWindow(), &windowWidth, &windowHeight);
        mWindowWidth = windowWidth;
        mWindowHeight = windowHeight;

        return true;
    }

    void UI::render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom)
    {
        // Calculate UI position at the bottom of the screen
        float uiY = mWindowHeight - (mHeight * mTileHeight);

        // Render the UI tilemap at the bottom
        TileMap::render(renderer, 0, -uiY, 1.0f);

        // Render collected items count
        const int itemSpacing = 64; // Space between items
        const int textOffsetY = 10; // Offset for text above the item icon
        const int itemStartX = 50;  // Starting X position for items
        int currentX = itemStartX;

        for (const auto &[itemId, count] : mCollectedItems)
        {
            // Calculate position based on the UI layout
            int x = currentX;
            int y = -uiY; // mWindowHeight - (mHeight * mTileHeight) + 16; // Align with the UI position

            // Render item icon
            const auto &tilesetInfo = mTilesetData->getTilesetInfo();
            int srcX = (itemId % tilesetInfo.columns) * tilesetInfo.tileWidth;
            int srcY = (itemId / tilesetInfo.columns) * tilesetInfo.tileHeight;

            renderer->renderTexture(mTileset,
                                    srcX, srcY, tilesetInfo.tileWidth, tilesetInfo.tileHeight,
                                    x, y,
                                    tilesetInfo.tileWidth, tilesetInfo.tileHeight);

            // Render count above the item
            std::stringstream ss;
            ss << "x" << count;
            renderer->renderText(ss.str(), x + tilesetInfo.tileWidth / 2, y - textOffsetY,
                                 {255, 255, 255, 255}); // White text

            // Move to next position
            currentX += itemSpacing;
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