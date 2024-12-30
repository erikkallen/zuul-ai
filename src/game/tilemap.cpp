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
        : mWidth(0), mHeight(0), mTileWidth(0), mTileHeight(0), mTilesetColumns(0), mDebugRendering(false)
    {
    }

    bool TileMap::loadFromFile(const std::string &mapPath, const std::string &tilesetPath, std::shared_ptr<Renderer> renderer)
    {
        try
        {
            // Load tileset
            std::ifstream tilesetFile(tilesetPath);
            if (!tilesetFile.is_open())
            {
                std::cerr << "Failed to open tileset file: " << tilesetPath << std::endl;
                return false;
            }

            json tilesetJson;
            tilesetFile >> tilesetJson;

            mTileWidth = tilesetJson["tilewidth"];
            mTileHeight = tilesetJson["tileheight"];
            mTilesetColumns = tilesetJson["columns"];

            std::string imagePath = "assets/" + std::string(tilesetJson["image"]);
            mTileset = renderer->loadTexture(imagePath);
            if (!mTileset)
            {
                std::cerr << "Failed to load tileset texture: " << imagePath << std::endl;
                return false;
            }

            // Load tile properties and animations from tileset
            if (tilesetJson.contains("tiles"))
            {
                for (const auto &tile : tilesetJson["tiles"])
                {
                    int tileId = tile["id"].get<int>();

                    // Check for collision boxes
                    if (tile.contains("objectgroup"))
                    {
                        const auto &objects = tile["objectgroup"]["objects"];
                        for (const auto &obj : objects)
                        {
                            if (obj.value("type", "") == "collision_box")
                            {
                                // Store collision box information
                                CollisionBox box;
                                box.x = obj.value("x", 0.0f);
                                box.y = obj.value("y", 0.0f);
                                box.width = obj.value("width", static_cast<float>(mTileWidth));
                                box.height = obj.value("height", static_cast<float>(mTileHeight));
                                mCollisionBoxes[tileId] = box;
                                break;
                            }
                        }
                    }

                    // Check for solid property
                    if (tile.contains("properties"))
                    {
                        const auto &properties = tile["properties"];
                        for (const auto &prop : properties)
                        {
                            if (prop["name"] == "solid" && prop["value"] == true)
                            {
                                mSolidTiles.insert(tileId);
                                break;
                            }
                        }
                    }

                    // Load animation data
                    if (tile.contains("animation"))
                    {
                        TileAnimation animation;
                        animation.currentFrame = 0;
                        animation.timer = 0;

                        for (const auto &frame : tile["animation"])
                        {
                            animation.tileIds.push_back(frame["tileid"].get<int>());
                            animation.durations.push_back(frame["duration"].get<float>() / 1000.0f);
                        }

                        mAnimatedTiles[tileId] = animation;
                    }
                }
            }

            // After loading all tiles, add this debug output
            if (!mSolidTiles.empty())
            {
                std::cout << "Tiles with solid property:" << std::endl;
                for (int tileId : mSolidTiles)
                {
                    std::cout << "  - Tile ID: " << tileId << std::endl;
                }
            }
            else
            {
                std::cout << "No tiles with solid property found." << std::endl;
            }

            // Also print collision boxes
            if (!mCollisionBoxes.empty())
            {
                std::cout << "Tiles with collision boxes:" << std::endl;
                for (const auto &[tileId, box] : mCollisionBoxes)
                {
                    std::cout << "  - Tile ID: " << tileId << " (box: x=" << box.x << ", y=" << box.y
                              << ", w=" << box.width << ", h=" << box.height << ")" << std::endl;
                }
            }
            else
            {
                std::cout << "No tiles with collision boxes found." << std::endl;
            }

            // Load map
            std::ifstream mapFile(mapPath);
            if (!mapFile.is_open())
            {
                std::cerr << "Failed to open map file: " << mapPath << std::endl;
                return false;
            }

            json mapJson;
            mapFile >> mapJson;

            mWidth = mapJson["width"];
            mHeight = mapJson["height"];

            // Load all layers
            const auto &layers = mapJson["layers"];
            if (layers.empty())
            {
                std::cerr << "No layers found in map file" << std::endl;
                return false;
            }

            for (const auto &layerJson : layers)
            {
                if (layerJson["type"] == "tilelayer") // Only process tile layers
                {
                    MapLayer layer;
                    layer.name = layerJson["name"];
                    layer.visible = layerJson.value("visible", true);

                    // Check for collision property
                    layer.collision = false;
                    if (layerJson.contains("properties"))
                    {
                        const auto &properties = layerJson["properties"];
                        for (const auto &prop : properties)
                        {
                            if (prop["name"] == "collision" && prop["value"].get<bool>())
                            {
                                layer.collision = true;
                                break;
                            }
                        }
                    }

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

    std::pair<int, int> TileMap::worldToTile(float x, float y) const
    {
        return {
            static_cast<int>(x) / mTileWidth,
            static_cast<int>(y) / mTileHeight};
    }

    bool TileMap::isSolid(float x, float y) const
    {
        auto [tileX, tileY] = worldToTile(x, y);
        if (tileX < 0 || tileX >= mWidth || tileY < 0 || tileY >= mHeight)
            return true; // Treat out-of-bounds as solid

        // Check each layer that has collision enabled
        for (const auto &layer : mLayers)
        {
            if (!layer.collision)
                continue;

            int tileId = layer.tileData[tileY * mWidth + tileX];
            if (tileId > 0) // Skip empty tiles
            {
                tileId--; // Convert to 0-based index

                // Check if tile is marked as solid
                if (mSolidTiles.find(tileId) != mSolidTiles.end())
                {
                    return true;
                }

                // Check if tile has a collision box
                auto boxIt = mCollisionBoxes.find(tileId);
                if (boxIt != mCollisionBoxes.end())
                {
                    const auto &box = boxIt->second;
                    float tileWorldX = tileX * mTileWidth;
                    float tileWorldY = tileY * mTileHeight;

                    // Check if point is within the collision box
                    if (x >= tileWorldX + box.x && x < tileWorldX + box.x + box.width &&
                        y >= tileWorldY + box.y && y < tileWorldY + box.y + box.height)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    bool TileMap::checkCollision(float x, float y, float width, float height) const
    {
        // Check all four corners of the bounding box
        return isSolid(x, y) ||                // Top-left
               isSolid(x + width, y) ||        // Top-right
               isSolid(x, y + height) ||       // Bottom-left
               isSolid(x + width, y + height); // Bottom-right
    }

    void TileMap::update(float deltaTime)
    {
        // Update all animated tiles
        for (auto &[tileId, animation] : mAnimatedTiles)
        {
            animation.timer += deltaTime;
            float currentDuration = animation.durations[animation.currentFrame];

            if (animation.timer >= currentDuration)
            {
                animation.timer -= currentDuration;
                animation.currentFrame = (animation.currentFrame + 1) % animation.tileIds.size();
            }
        }
    }

    int TileMap::getCurrentTileId(int tileId) const
    {
        // Check if this tile has an animation
        auto it = mAnimatedTiles.find(tileId - 1); // Subtract 1 because Tiled uses 1-based indices
        if (it != mAnimatedTiles.end())
        {
            // Return the current frame's tile ID
            return it->second.tileIds[it->second.currentFrame];
        }
        return tileId - 1; // Return the original tile ID (converted to 0-based)
    }

    void TileMap::renderLayer(const MapLayer &layer, std::shared_ptr<Renderer> renderer, float offsetX, float offsetY)
    {
        if (!layer.visible)
            return;

        for (int y = 0; y < mHeight; ++y)
        {
            for (int x = 0; x < mWidth; ++x)
            {
                int tileId = layer.tileData[y * mWidth + x];
                if (tileId > 0) // Tiled uses 0 for empty tiles
                {
                    int currentTileId = getCurrentTileId(tileId);
                    int srcX = (currentTileId % mTilesetColumns) * mTileWidth;
                    int srcY = (currentTileId / mTilesetColumns) * mTileHeight;

                    float destX = x * mTileWidth + offsetX;
                    float destY = y * mTileHeight + offsetY;

                    // Only render tiles that are visible on screen
                    if (destX + mTileWidth >= 0 && destX < 800 && // 800 is window width
                        destY + mTileHeight >= 0 && destY < 600)  // 600 is window height
                    {
                        renderer->renderTexture(mTileset,
                                                srcX, srcY, mTileWidth, mTileHeight,
                                                static_cast<int>(destX), static_cast<int>(destY),
                                                mTileWidth, mTileHeight);
                    }
                }
            }
        }
    }

    void TileMap::renderDebugCollisions(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY)
    {
        std::cout << "\nStarting debug collision rendering..." << std::endl;
        std::cout << "Number of layers: " << mLayers.size() << std::endl;

        // Render collision boxes and solid tiles for all layers
        for (const auto &layer : mLayers)
        {
            std::cout << "Checking layer: " << layer.name << std::endl;

            // Check all tiles in the layer
            for (int y = 0; y < mHeight; ++y)
            {
                for (int x = 0; x < mWidth; ++x)
                {
                    int globalTileId = layer.tileData[y * mWidth + x];
                    if (globalTileId > 0) // Skip empty tiles
                    {
                        int localTileId = globalTileId - 1; // Convert global to local tile ID
                        float tileWorldX = x * mTileWidth + offsetX;
                        float tileWorldY = y * mTileHeight + offsetY;

                        // Only render debug info for visible tiles
                        if (tileWorldX + mTileWidth >= 0 && tileWorldX < 800 &&
                            tileWorldY + mTileHeight >= 0 && tileWorldY < 600)
                        {
                            std::cout << "Checking tile at (" << x << "," << y << ") - Global ID: " << globalTileId
                                      << ", Local ID: " << localTileId << std::endl;

                            // Draw red border for solid tiles
                            if (mSolidTiles.find(localTileId) != mSolidTiles.end())
                            {
                                std::cout << "  Drawing solid tile border at (" << tileWorldX << "," << tileWorldY
                                          << ") size: " << mTileWidth << "x" << mTileHeight << std::endl;
                                renderer->renderRect(
                                    static_cast<int>(tileWorldX),
                                    static_cast<int>(tileWorldY),
                                    mTileWidth,
                                    mTileHeight,
                                    255, 0, 0, 255 // Red
                                );
                            }

                            // Draw yellow border for collision boxes
                            auto boxIt = mCollisionBoxes.find(localTileId);
                            if (boxIt != mCollisionBoxes.end())
                            {
                                const auto &box = boxIt->second;
                                std::cout << "  Drawing collision box at (" << (tileWorldX + box.x) << ","
                                          << (tileWorldY + box.y) << ") size: " << box.width << "x" << box.height << std::endl;
                                renderer->renderRect(
                                    static_cast<int>(tileWorldX + box.x),
                                    static_cast<int>(tileWorldY + box.y),
                                    static_cast<int>(box.width),
                                    static_cast<int>(box.height),
                                    255, 255, 0, 255 // Yellow
                                );
                            }
                        }
                    }
                }
            }
        }
    }

    void TileMap::render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY)
    {
        // Render all layers in order
        for (const auto &layer : mLayers)
        {
            renderLayer(layer, renderer, offsetX, offsetY);
        }
    }

} // namespace zuul