#include "game/tilemap.hpp"
#include <nlohmann/json.hpp>
#include <engine/renderer.hpp>
#include <fstream>
#include <iostream>
#include <memory>

using json = nlohmann::json;

namespace zuul
{
    TileMap::TileMap()
        : mWidth(0), mHeight(0), mTileWidth(0), mTileHeight(0), mTilesetColumns(0)
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

            // Load animated tiles from tileset
            if (tilesetJson.contains("tiles"))
            {
                for (const auto &tile : tilesetJson["tiles"])
                {
                    if (tile.contains("animation"))
                    {
                        int tileId = tile["id"].get<int>();
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
                    layer.visible = layerJson.value("visible", true); // Default to visible if not specified
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

    void TileMap::render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY)
    {
        // Render all layers in order
        for (const auto &layer : mLayers)
        {
            renderLayer(layer, renderer, offsetX, offsetY);
        }
    }

} // namespace zuul