#pragma once

#include <engine/game.hpp>
#include <game/tilemap.hpp>
#include <game/player.hpp>
#include <memory>

namespace zuul
{

    class ZuulGame : public Game
    {
    public:
        bool initialize(int windowWidth, int windowHeight, const ::std::string &windowTitle) override;

    protected:
        void update(float deltaTime) override;
        void render() override;

    private:
        ::std::unique_ptr<TileMap> mTileMap;
        ::std::unique_ptr<Player> mPlayer;
    };

} // namespace zuul