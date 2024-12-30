#pragma once

namespace zuul
{

    class Camera
    {
    public:
        Camera(int windowWidth, int windowHeight, int mapWidth, int mapHeight);

        void update(float targetX, float targetY);

        // Get the offset to apply to rendered objects
        float getOffsetX() const { return mOffsetX; }
        float getOffsetY() const { return mOffsetY; }

        // Convert world coordinates to screen coordinates
        void worldToScreen(float worldX, float worldY, float &screenX, float &screenY) const;

        // Convert screen coordinates to world coordinates
        void screenToWorld(float screenX, float screenY, float &worldX, float &worldY) const;

    private:
        float mOffsetX;
        float mOffsetY;
        int mWindowWidth;
        int mWindowHeight;
        int mMapWidth;
        int mMapHeight;
    };

} // namespace zuul