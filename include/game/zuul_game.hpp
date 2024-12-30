#pragma once

#include <memory>
#include <string>
#include <engine/game.hpp>
#include <game/tilemap.hpp>
#include <game/player.hpp>
#include <game/camera.hpp>

namespace zuul
{

    class ZuulGame : public Game
    {
    public:
        ZuulGame() = default;
        ~ZuulGame() = default;

        bool initialize(int windowWidth, int windowHeight, const std::string &windowTitle) override;
        void update(float deltaTime) override;
        void render() override;

    private:
        std::unique_ptr<TileMap> mTileMap;
        std::unique_ptr<Player> mPlayer;
        std::unique_ptr<Camera> mCamera;
        bool mDebugRendering = false;
        int mWindowWidth;
        int mWindowHeight;
    };

} // namespace zuul