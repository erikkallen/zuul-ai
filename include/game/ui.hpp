#pragma once

#include <memory>
#include <string>
#include <map>
#include <engine/renderer.hpp>
#include <game/tilemap.hpp>

namespace zuul
{
    class UI : public TileMap
    {
    public:
        UI();
        ~UI() override = default;

        bool initialize(std::shared_ptr<Renderer> renderer);
        void render(std::shared_ptr<Renderer> renderer, float offsetX = 0.0f, float offsetY = 0.0f, float zoom = 1.0f) override;

        // Item collection
        void addCollectedItem(int itemId);
        int getCollectedItemCount(int itemId) const;

    private:
        std::map<int, int> mCollectedItems; // itemId -> count
        int mWindowWidth;
        int mWindowHeight;
    };

} // namespace zuul