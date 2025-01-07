#include <game/tileset_data.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace zuul
{

    bool TilesetData::loadFromFile(const std::string &filepath, std::shared_ptr<Renderer> renderer)
    {
        try
        {
            std::ifstream file(filepath);
            if (!file.is_open())
            {
                std::cerr << "Failed to open tileset file: " << filepath << std::endl;
                return false;
            }

            json tilesetJson;
            file >> tilesetJson;

            // Load tileset info
            mTilesetInfo.columns = tilesetJson["columns"].get<int>();
            mTilesetInfo.tileWidth = tilesetJson["tilewidth"].get<int>();
            mTilesetInfo.tileHeight = tilesetJson["tileheight"].get<int>();
            mTilesetInfo.imagePath = tilesetJson["image"].get<std::string>();

            // Load the tileset texture
            std::string fullImagePath = "assets/" + mTilesetInfo.imagePath;
            mTexture = renderer->loadTexture(fullImagePath);
            if (!mTexture)
            {
                std::cerr << "Failed to load tileset texture: " << fullImagePath << std::endl;
                return false;
            }

            // Process each tile's data
            const auto &tiles = tilesetJson["tiles"];
            for (const auto &tile : tiles)
            {
                int id = tile["id"].get<int>();

                // Load animation data
                if (tile.contains("animation"))
                {
                    TileAnimation animation;
                    for (const auto &frame : tile["animation"])
                    {
                        AnimationFrame newFrame;
                        newFrame.tileId = frame["tileid"].get<int>();
                        newFrame.duration = frame["duration"].get<float>() / 1000.0f;
                        animation.frames.push_back(newFrame);
                    }
                    mAnimations[id] = animation;
                }

                // Load collision data
                if (tile.contains("objectgroup") && tile["objectgroup"].contains("objects"))
                {
                    const auto &objects = tile["objectgroup"]["objects"];
                    for (const auto &obj : objects)
                    {
                        if (obj["name"] == "collision_box")
                        {
                            CollisionBox box;
                            box.x = obj["x"].get<float>();
                            box.y = obj["y"].get<float>();
                            box.width = obj["width"].get<float>();
                            box.height = obj["height"].get<float>();
                            mCollisionBoxes[id] = box;
                        }
                    }
                }

                // Load solid property
                if (tile.contains("properties"))
                {
                    for (const auto &prop : tile["properties"])
                    {
                        if (prop["name"] == "solid" && prop["type"] == "bool")
                        {
                            mSolidTiles[id] = prop["value"].get<bool>();
                        }
                    }
                }
            }

            return true;
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error loading tileset data: " << e.what() << std::endl;
            return false;
        }
    }

    void TilesetData::update(float deltaTime)
    {
        for (auto &[id, animation] : mAnimations)
        {
            if (!animation.frames.empty())
            {
                animation.timer += deltaTime;
                float currentFrameDuration = animation.frames[animation.currentFrameIndex].duration;

                while (animation.timer >= currentFrameDuration)
                {
                    animation.timer -= currentFrameDuration;
                    animation.currentFrameIndex = (animation.currentFrameIndex + 1) % animation.frames.size();
                }
            }
        }
    }

    bool TilesetData::hasAnimation(int tileId) const
    {
        return mAnimations.find(tileId) != mAnimations.end();
    }

    int TilesetData::getCurrentTileId(int baseTileId) const
    {
        auto it = mAnimations.find(baseTileId);
        if (it != mAnimations.end() && !it->second.frames.empty())
        {
            return it->second.frames[it->second.currentFrameIndex].tileId;
        }
        return baseTileId;
    }

    const TileAnimation *TilesetData::getAnimation(int tileId) const
    {
        auto it = mAnimations.find(tileId);
        return it != mAnimations.end() ? &it->second : nullptr;
    }

    bool TilesetData::hasCollisionBox(int tileId) const
    {
        return mCollisionBoxes.find(tileId) != mCollisionBoxes.end();
    }

    const CollisionBox *TilesetData::getCollisionBox(int tileId) const
    {
        auto it = mCollisionBoxes.find(tileId);
        return it != mCollisionBoxes.end() ? &it->second : nullptr;
    }

    bool TilesetData::isSolid(int tileId) const
    {
        auto it = mSolidTiles.find(tileId);
        return it != mSolidTiles.end() ? it->second : false;
    }

} // namespace zuul