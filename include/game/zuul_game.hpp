#pragma once

#include <engine/game.hpp>
#include <game/tilemap.hpp>
#include <memory>

namespace zuul
{

    class ZuulGame : public Game
    {
    public:
        bool initialize(int windowWidth, int windowHeight, const ::std::string &windowTitle);

    protected:
        void update(float deltaTime) override;
        void render() override;

    private:
        ::std::unique_ptr<TileMap> mTileMap;
    };
} // namespace zuul