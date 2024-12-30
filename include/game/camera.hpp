#pragma once

namespace zuul
{

    class Camera
    {
    public:
        Camera(int windowWidth, int windowHeight, int mapWidth, int mapHeight);

        void update(float targetX, float targetY);
        void setZoom(float zoom);
        void adjustZoom(float delta); // For incrementally changing zoom

        // Get the offset to apply to rendered objects
        float getOffsetX() const { return mOffsetX; }
        float getOffsetY() const { return mOffsetY; }
        float getZoom() const { return mZoom; }

        // Convert world coordinates to screen coordinates
        void worldToScreen(float worldX, float worldY, float &screenX, float &screenY) const;

        // Convert screen coordinates to world coordinates
        void screenToWorld(float screenX, float screenY, float &worldX, float &worldY) const;

    private:
        float mOffsetX;
        float mOffsetY;
        float mZoom;
        int mWindowWidth;
        int mWindowHeight;
        int mMapWidth;
        int mMapHeight;

        void clampOffset(); // Helper to keep camera within map bounds
    };

} // namespace zuul