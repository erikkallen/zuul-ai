#include <game/camera.hpp>
#include <algorithm>

namespace zuul
{

    Camera::Camera(int windowWidth, int windowHeight, int mapWidth, int mapHeight)
        : mOffsetX(0), mOffsetY(0), mZoom(1.0f),
          mWindowWidth(windowWidth), mWindowHeight(windowHeight),
          mMapWidth(mapWidth), mMapHeight(mapHeight)
    {
    }

    void Camera::update(float targetX, float targetY)
    {
        // Center the camera on the target, taking zoom into account
        float halfScreenWidth = mWindowWidth / (2.0f * mZoom);
        float halfScreenHeight = mWindowHeight / (2.0f * mZoom);

        mOffsetX = targetX - halfScreenWidth;
        mOffsetY = targetY - halfScreenHeight;

        clampOffset();
    }

    void Camera::setZoom(float zoom)
    {
        // Store old zoom for position adjustment
        float oldZoom = mZoom;

        // Clamp zoom between 1.0 and 3.0
        mZoom = std::clamp(zoom, 1.0f, 3.0f);

        // Adjust offset to maintain center point when zooming
        if (oldZoom != mZoom)
        {
            float centerX = mOffsetX + mWindowWidth / (2.0f * oldZoom);
            float centerY = mOffsetY + mWindowHeight / (2.0f * oldZoom);

            mOffsetX = centerX - mWindowWidth / (2.0f * mZoom);
            mOffsetY = centerY - mWindowHeight / (2.0f * mZoom);

            clampOffset();
        }
    }

    void Camera::adjustZoom(float delta)
    {
        setZoom(mZoom + delta);
    }

    void Camera::worldToScreen(float worldX, float worldY, float &screenX, float &screenY) const
    {
        screenX = (worldX - mOffsetX) * mZoom;
        screenY = (worldY - mOffsetY) * mZoom;
    }

    void Camera::screenToWorld(float screenX, float screenY, float &worldX, float &worldY) const
    {
        worldX = screenX / mZoom + mOffsetX;
        worldY = screenY / mZoom + mOffsetY;
    }

    void Camera::clampOffset()
    {
        // Calculate maximum offsets based on zoom level
        float maxOffsetX = mMapWidth - mWindowWidth / mZoom;
        float maxOffsetY = mMapHeight - mWindowHeight / mZoom;

        mOffsetX = std::clamp(mOffsetX, 0.0f, maxOffsetX);
        mOffsetY = std::clamp(mOffsetY, 0.0f, maxOffsetY);
    }

} // namespace zuul