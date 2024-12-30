#pragma once

#include <engine/renderer.hpp>
#include <memory>
#include <vector>
#include <string>

namespace zuul
{
    struct TileSet
    {
        ::std::shared_ptr<Texture> texture;
        int tileWidth;
        int tileHeight;
        int columns;
        int tileCount;
    };

    class TileMap
    {
    public:
        bool loadFromFile(const ::std::string &mapPath, const ::std::string &tilesetPath, ::std::shared_ptr<Renderer> renderer);
        void render(::std::shared_ptr<Renderer> renderer);

    private:
        TileSet mTileset;
        ::std::vector<int> mTileData;
        int mWidth;
        int mHeight;
    };
} // namespace zuul