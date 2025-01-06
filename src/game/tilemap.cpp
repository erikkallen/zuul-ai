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
          mWindowWidth(800), mWindowHeight(600), // Default window dimensions
          mDebugRendering(false)
    {
    }

    bool TileMap::loadFromFile(const std::string &filepath, std::shared_ptr<Renderer> renderer)
    {
        try
        {
            std::ifstream file(filepath);
            if (!file.is_open())
            {
                std::cerr << "Failed to open map file: " << filepath << std::endl;
                return false;
            }

            json mapData;
            file >> mapData;

            mWidth = mapData["width"].get<int>();
            mHeight = mapData["height"].get<int>();
            mTileWidth = mapData["tileheight"].get<int>();
            mTileHeight = mapData["tilewidth"].get<int>();

            // Load tilesets
            int firstGid = 1;
            std::string tilesetPath;
            for (const auto &tileset : mapData["tilesets"])
            {
                if (tileset.contains("source"))
                {
                    std::string source = tileset["source"].get<std::string>();
                    if (source.find("map_tiles.tsj") != std::string::npos)
                    {
                        firstGid = tileset["firstgid"].get<int>();
                        tilesetPath = "assets/map_tiles.tsj";
                        break;
                    }
                }
            }

            // Create and load tileset data
            mTilesetData = std::make_shared<TilesetData>();
            if (!mTilesetData->loadFromFile(tilesetPath))
            {
                std::cerr << "Failed to load tileset data" << std::endl;
                return false;
            }

            // Load tileset texture
            mTileset = renderer->loadTexture("assets/map_tiles.png");
            if (!mTileset)
            {
                std::cerr << "Failed to load tileset texture" << std::endl;
                return false;
            }

            // Clear existing layers and items
            mLayers.clear();
            mItems.clear();

            // Load layers
            for (const auto &layer : mapData["layers"])
            {
                std::string type = layer["type"].get<std::string>();

                if (type == "tilelayer")
                {
                    MapLayer newLayer;
                    newLayer.name = layer["name"].get<std::string>();
                    newLayer.visible = layer["visible"].get<bool>();

                    // Load tile data
                    const auto &data = layer["data"];
                    newLayer.tileData.reserve(data.size());
                    for (const auto &gid : data)
                    {
                        newLayer.tileData.push_back(gid.get<unsigned int>());
                    }

                    mLayers.push_back(newLayer);
                }
                else if (type == "objectgroup")
                {
                    // Load items
                    for (const auto &obj : layer["objects"])
                    {
                        if (obj.contains("type") && obj["type"].get<std::string>() == "Item")
                        {
                            int gid = obj["gid"].get<int>();
                            float x = obj["x"].get<float>();
                            float y = obj["y"].get<float>() - mTileHeight; // Adjust Y position for Tiled's coordinate system

                            // Convert GID to local tile ID by subtracting firstGid
                            int localTileId = gid - firstGid;

                            mItems.emplace_back(localTileId, x, y, mTilesetData, mTileset);
                        }
                    }
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
        // Update animations in tileset
        mTilesetData->update(deltaTime);

        // Update items
        for (auto &item : mItems)
        {
            item.update(deltaTime);
        }
    }

    void TileMap::render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom)
    {
        // Calculate visible tile range based on zoom and offset
        int startTileX = static_cast<int>(offsetX / (mTileWidth * zoom));
        int startTileY = static_cast<int>(offsetY / (mTileHeight * zoom));
        int endTileX = static_cast<int>((offsetX + mWindowWidth / zoom) / mTileWidth) + 2;   // Add extra column
        int endTileY = static_cast<int>((offsetY + mWindowHeight / zoom) / mTileHeight) + 2; // Add extra row

        // Clamp to map bounds
        startTileX = std::max(0, startTileX);
        startTileY = std::max(0, startTileY);
        endTileX = std::min(mWidth, endTileX);
        endTileY = std::min(mHeight, endTileY);

        // Constants for Tiled's tile flags
        const unsigned FLIPPED_HORIZONTALLY_FLAG = 0x80000000;
        const unsigned FLIPPED_VERTICALLY_FLAG = 0x40000000;
        const unsigned FLIPPED_DIAGONALLY_FLAG = 0x20000000;
        const unsigned ALL_FLAGS = FLIPPED_HORIZONTALLY_FLAG | FLIPPED_VERTICALLY_FLAG | FLIPPED_DIAGONALLY_FLAG;

        // Render each visible layer
        for (const auto &layer : mLayers)
        {
            if (layer.visible)
            {
                for (int y = startTileY; y < endTileY; ++y)
                {
                    for (int x = startTileX; x < endTileX; ++x)
                    {
                        unsigned int gid = layer.tileData[y * mWidth + x];
                        if (gid > 0)
                        {
                            // Extract the actual tile ID (remove flip flags)
                            int tileId = (gid & ~ALL_FLAGS) - 1; // Convert to 0-based

                            // Get current animation frame if tile is animated
                            if (mTilesetData->hasAnimation(tileId))
                            {
                                tileId = mTilesetData->getCurrentTileId(tileId);
                            }

                            // Calculate source rectangle in tileset
                            const auto &tilesetInfo = mTilesetData->getTilesetInfo();
                            int srcX = (tileId % tilesetInfo.columns) * mTileWidth;
                            int srcY = (tileId / tilesetInfo.columns) * mTileHeight;

                            // Calculate destination rectangle with zoom
                            // Use floor for position and ceil for dimensions to prevent gaps
                            float destX = std::floor((x * mTileWidth - offsetX) * zoom);
                            float destY = std::floor((y * mTileHeight - offsetY) * zoom);
                            int destW = static_cast<int>(std::ceil((x + 1) * mTileWidth * zoom) - std::floor(x * mTileWidth * zoom));
                            int destH = static_cast<int>(std::ceil((y + 1) * mTileHeight * zoom) - std::floor(y * mTileHeight * zoom));

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

        // Render items
        renderItems(renderer, offsetX, offsetY, zoom);
    }

    void TileMap::renderItems(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom)
    {
        for (auto &item : mItems)
        {
            item.render(renderer, offsetX, offsetY, zoom);
        }
    }

    void TileMap::checkItemCollisions(float x, float y, float width, float height) const
    {
        for (auto &item : mItems)
        {
            if (!item.isCollected() && item.isColliding(x, y, width, height))
            {
                item.collect();
                // TODO: Add scoring or other item collection effects
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