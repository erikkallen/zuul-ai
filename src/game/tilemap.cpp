#include "game/tilemap.hpp"
#include <nlohmann/json.hpp>
#include <engine/renderer.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <algorithm>

using json = nlohmann::json;

namespace zuul
{
    TileMap::TileMap()
        : mWidth(0), mHeight(0), mTileWidth(0), mTileHeight(0),
          mWindowWidth(800), mWindowHeight(600) // Default window dimensions
    {
        mTilesetData = std::make_unique<TilesetData>();
    }

    bool TileMap::loadFromFile(const std::string &mapFile, const std::string &tilesetFile, std::shared_ptr<Renderer> renderer)
    {
        try
        {
            // Load tileset data (TSJ) first to get the image path
            if (!mTilesetData->loadFromFile(tilesetFile))
            {
                std::cerr << "Failed to load tileset data: " << tilesetFile << std::endl;
                return false;
            }

            // Load tileset JSON to get image path and columns
            std::ifstream tilesetJson(tilesetFile);
            if (!tilesetJson.is_open())
            {
                std::cerr << "Failed to open tileset file: " << tilesetFile << std::endl;
                return false;
            }

            json tsj;
            tilesetJson >> tsj;
            std::string imagePath = "assets/" + std::string(tsj["image"]);
            mTilesetColumns = tsj["columns"].get<int>();

            // Load tileset texture using image path from TSJ
            mTileset = renderer->loadTexture(imagePath);
            if (!mTileset)
            {
                std::cerr << "Failed to load tileset texture: " << imagePath << std::endl;
                return false;
            }

            // Load map data
            std::ifstream file(mapFile);
            if (!file.is_open())
            {
                std::cerr << "Failed to open map file: " << mapFile << std::endl;
                return false;
            }

            json mapJson;
            file >> mapJson;

            // Read map dimensions
            mWidth = mapJson["width"];
            mHeight = mapJson["height"];
            mTileWidth = mapJson["tilewidth"];
            mTileHeight = mapJson["tileheight"];

            // Load layers
            const auto &layers = mapJson["layers"];
            for (const auto &layerJson : layers)
            {
                if (layerJson["type"] == "tilelayer")
                {
                    MapLayer layer;
                    layer.name = layerJson["name"];
                    layer.visible = layerJson.value("visible", true);
                    layer.tileData = layerJson["data"].get<std::vector<int>>();
                    mLayers.push_back(layer);
                }
            }

            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error loading map: " << e.what() << std::endl;
            return false;
        }
    }

    void TileMap::update(float deltaTime)
    {
        mTilesetData->update(deltaTime);
    }

    void TileMap::render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom)
    {
        // Calculate visible tile range based on zoom and offset
        int startTileX = static_cast<int>(offsetX / (mTileWidth * zoom));
        int startTileY = static_cast<int>(offsetY / (mTileHeight * zoom));
        int endTileX = static_cast<int>((offsetX + mWindowWidth / zoom) / mTileWidth) + 2;   // Add extra column
        int endTileY = static_cast<int>((offsetY + mWindowHeight / zoom) / mTileHeight) + 2; // Add extra row

        // Clamp tile range to map bounds
        startTileX = std::max(0, startTileX);
        startTileY = std::max(0, startTileY);
        endTileX = std::min(mWidth, endTileX);
        endTileY = std::min(mHeight, endTileY);

        // Render each visible layer
        for (const auto &layer : mLayers)
        {
            if (layer.visible)
            {
                // Only render tiles within the visible range
                for (int y = startTileY; y < endTileY; ++y)
                {
                    for (int x = startTileX; x < endTileX; ++x)
                    {
                        int tileId = layer.tileData[y * mWidth + x];
                        if (tileId > 0)
                        {
                            int currentTileId = mTilesetData->getCurrentTileId(tileId - 1);
                            int srcX = (currentTileId % mTilesetColumns) * mTileWidth;
                            int srcY = (currentTileId / mTilesetColumns) * mTileHeight;

                            float destX = std::floor((x * mTileWidth - offsetX) * zoom);
                            float destY = std::floor((y * mTileHeight - offsetY) * zoom);
                            int destW = static_cast<int>(std::ceil(mTileWidth * zoom));
                            int destH = static_cast<int>(std::ceil(mTileHeight * zoom));

                            renderer->renderTexture(mTileset,
                                                    srcX, srcY, mTileWidth, mTileHeight,
                                                    static_cast<int>(destX),
                                                    static_cast<int>(destY),
                                                    destW, destH);
                        }
                    }
                }
            }
        }
    }

    void TileMap::renderDebugCollisions(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom)
    {
        // Calculate visible tile range based on zoom and offset
        int startTileX = static_cast<int>(offsetX / (mTileWidth * zoom));
        int startTileY = static_cast<int>(offsetY / (mTileHeight * zoom));
        int endTileX = static_cast<int>((offsetX + mWindowWidth / zoom) / mTileWidth) + 1;
        int endTileY = static_cast<int>((offsetY + mWindowHeight / zoom) / mTileHeight) + 1;

        // Clamp tile range to map bounds
        startTileX = std::max(0, startTileX);
        startTileY = std::max(0, startTileY);
        endTileX = std::min(mWidth, endTileX);
        endTileY = std::min(mHeight, endTileY);

        // Render debug info for each visible layer
        for (const auto &layer : mLayers)
        {
            if (!layer.visible)
                continue;

            for (int y = startTileY; y < endTileY; ++y)
            {
                for (int x = startTileX; x < endTileX; ++x)
                {
                    int globalTileId = layer.tileData[y * mWidth + x];
                    if (globalTileId > 0)
                    {
                        int localTileId = globalTileId - 1;
                        float tileWorldX = (x * mTileWidth - offsetX) * zoom;
                        float tileWorldY = (y * mTileHeight - offsetY) * zoom;

                        // Draw red border for solid tiles
                        if (mTilesetData->isSolid(localTileId))
                        {
                            renderer->renderRect(
                                static_cast<int>(tileWorldX),
                                static_cast<int>(tileWorldY),
                                static_cast<int>(mTileWidth * zoom),
                                static_cast<int>(mTileHeight * zoom),
                                255, 0, 0, 255 // Red
                            );
                        }

                        // Draw yellow border for collision boxes
                        const auto *box = mTilesetData->getCollisionBox(localTileId);
                        if (box != nullptr)
                        {
                            renderer->renderRect(
                                static_cast<int>((x * mTileWidth + box->x - offsetX) * zoom),
                                static_cast<int>((y * mTileHeight + box->y - offsetY) * zoom),
                                static_cast<int>(box->width * zoom),
                                static_cast<int>(box->height * zoom),
                                255, 255, 0, 255 // Yellow
                            );
                        }
                    }
                }
            }
        }
    }

    bool TileMap::checkCollision(float x, float y, float width, float height) const
    {
        // Constants for Tiled's tile flags
        const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
        const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
        const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;
        const unsigned ALL_FLAGS = FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG;

        // Convert world coordinates to tile coordinates
        int startTileX = static_cast<int>(x / mTileWidth);
        int startTileY = static_cast<int>(y / mTileHeight);
        int endTileX = static_cast<int>((x + width - 1) / mTileWidth); // -1 to make it inclusive
        int endTileY = static_cast<int>((y + height - 1) / mTileHeight);

        // Check each potentially colliding tile
        for (int tileY = startTileY; tileY <= endTileY; ++tileY)
        {
            for (int tileX = startTileX; tileX <= endTileX; ++tileX)
            {
                if (tileX >= 0 && tileX < mWidth && tileY >= 0 && tileY < mHeight)
                {
                    for (const auto &layer : mLayers)
                    {
                        unsigned int gid = layer.tileData[tileY * mWidth + tileX];
                        if (gid > 0)
                        {
                            // Extract the actual tile ID (remove flip flags)
                            int tileId = (gid & ~ALL_FLAGS) - 1; // Convert to 0-based

                            // Check if tile is solid
                            if (mTilesetData->isSolid(tileId))
                            {
                                // Do a precise AABB collision check
                                float tileLeft = tileX * mTileWidth;
                                float tileRight = tileLeft + mTileWidth - 1; // -1 for inclusive bounds
                                float tileTop = tileY * mTileHeight;
                                float tileBottom = tileTop + mTileHeight - 1;

                                if (x <= tileRight && x + width - 1 >= tileLeft &&
                                    y <= tileBottom && y + height - 1 >= tileTop)
                                {
                                    return true;
                                }
                            }

                            // Check collision box if present
                            if (const CollisionBox *box = mTilesetData->getCollisionBox(tileId))
                            {
                                float boxLeft = tileX * mTileWidth + box->x;
                                float boxRight = boxLeft + box->width - 1; // -1 for inclusive bounds
                                float boxTop = tileY * mTileHeight + box->y;
                                float boxBottom = boxTop + box->height - 1;

                                if (x <= boxRight && x + width - 1 >= boxLeft &&
                                    y <= boxBottom && y + height - 1 >= boxTop)
                                {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    std::pair<int, int> TileMap::worldToTile(float x, float y) const
    {
        return {static_cast<int>(x / mTileWidth), static_cast<int>(y / mTileHeight)};
    }

} // namespace zuul