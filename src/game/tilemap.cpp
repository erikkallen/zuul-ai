#include "game/tilemap.hpp"
#include <nlohmann/json.hpp>
#include <engine/renderer.hpp>
#include <fstream>
#include <iostream>
#include <memory>

using json = nlohmann::json;

namespace zuul
{

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

            mTileset.tileWidth = tilesetJson["tilewidth"];
            mTileset.tileHeight = tilesetJson["tileheight"];
            mTileset.columns = tilesetJson["columns"];
            mTileset.tileCount = tilesetJson["tilecount"];

            std::string imagePath = "assets/" + std::string(tilesetJson["image"]);
            mTileset.texture = renderer->loadTexture(imagePath);
            if (!mTileset.texture)
            {
                std::cerr << "Failed to load tileset texture: " << imagePath << std::endl;
                return false;
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

            // Get the first layer's data (assuming it's the background layer)
            const auto &layers = mapJson["layers"];
            if (layers.empty())
            {
                std::cerr << "No layers found in map file" << std::endl;
                return false;
            }

            const auto &firstLayer = layers[0];
            mTileData = firstLayer["data"].get<std::vector<int>>();

            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error loading map: " << e.what() << std::endl;
            return false;
        }
    }

    void TileMap::render(std::shared_ptr<Renderer> renderer)
    {
        for (int y = 0; y < mHeight; ++y)
        {
            for (int x = 0; x < mWidth; ++x)
            {
                int tileIndex = mTileData[y * mWidth + x] - 1; // Tiled uses 1-based indices
                if (tileIndex >= 0)
                {
                    int srcX = (tileIndex % mTileset.columns) * mTileset.tileWidth;
                    int srcY = (tileIndex / mTileset.columns) * mTileset.tileHeight;

                    renderer->renderTexture(mTileset.texture,
                                            srcX, srcY, mTileset.tileWidth, mTileset.tileHeight,
                                            x * mTileset.tileWidth, y * mTileset.tileHeight,
                                            mTileset.tileWidth, mTileset.tileHeight);
                }
            }
        }
    }

} // namespace zuul