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
        // Calculate zoom to stretch UI to window width
        float stretchZoom = static_cast<float>(mWindowWidth) / (mWidth * mTileWidth);

        // Calculate UI position at the bottom of the screen
        float uiHeight = mHeight * mTileHeight * stretchZoom;
        float renderY = mWindowHeight - uiHeight;

        std::cout << "Window Width: " << mWindowWidth
                  << ", UI Width: " << (mWidth * mTileWidth)
                  << ", Stretch Zoom: " << stretchZoom
                  << ", UI Height: " << uiHeight
                  << ", Render Y: " << renderY << std::endl;

        // Render the UI tilemap at the bottom
        for (const auto &layer : mLayers)
        {
            if (layer.visible)
            {
                for (int y = 0; y < mHeight; ++y)
                {
                    for (int x = 0; x < mWidth; ++x)
                    {
                        unsigned int tileId = layer.tileData[y * mWidth + x];
                        if (tileId > 0)
                        {
                            // Convert from Tiled's 1-based indices to 0-based
                            tileId--;

                            // Calculate source rectangle in tileset
                            int srcX = (tileId % mTilesetData->getTilesetInfo().columns) * mTileWidth;
                            int srcY = (tileId / mTilesetData->getTilesetInfo().columns) * mTileHeight;

                            // Calculate destination position
                            float destX = x * mTileWidth * stretchZoom;
                            float destY = renderY + (y * mTileHeight * stretchZoom);

                            // Render the tile
                            renderer->renderTexture(mTileset,
                                                    srcX, srcY, mTileWidth, mTileHeight,
                                                    static_cast<int>(destX),
                                                    static_cast<int>(destY),
                                                    static_cast<int>(mTileWidth * stretchZoom),
                                                    static_cast<int>(mTileHeight * stretchZoom));
                        }
                    }
                }
            }
        }

        // Render collected items count
        const int itemSpacing = 64 * stretchZoom; // Space between items, scaled with zoom
        const int textOffsetY = 16 * stretchZoom; // Offset for text above the item icon, scaled with zoom
        const int itemStartX = 50 * stretchZoom;  // Starting X position for items, scaled with zoom
        int currentX = itemStartX;

        for (const auto &[itemId, count] : mCollectedItems)
        {
            // Calculate position based on the UI layout
            float x = currentX;
            float y = renderY + (mTileHeight * stretchZoom * 0.5f); // Position items in the UI bar

            // Render item icon using the new renderTile method
            mTilesetData->renderTile(renderer, itemId, x, y - textOffsetY);

            // Render count above the item
            std::stringstream ss;
            ss << "x" << count;
            renderer->renderText(ss.str(),
                                 static_cast<int>(x + (mTileWidth * stretchZoom * 0.5f)),
                                 static_cast<int>(y - textOffsetY),
                                 {255, 255, 255, 255}); // White text

            // Move to next position
            currentX += itemSpacing;
        }
    }

    void UI::addCollectedItem(int itemId)
    {
        std::cout << "Adding item to collection: " << itemId << std::endl;
        mCollectedItems[itemId]++;
        std::cout << "Current count for item " << itemId << ": " << mCollectedItems[itemId] << std::endl;
    }

    int UI::getCollectedItemCount(int itemId) const
    {
        auto it = mCollectedItems.find(itemId);
        return it != mCollectedItems.end() ? it->second : 0;
    }

} // namespace zuul