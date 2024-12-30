#include <game/camera.hpp>
#include <algorithm>

namespace zuul
{

    Camera::Camera(int windowWidth, int windowHeight, int mapWidth, int mapHeight)
        : mOffsetX(0), mOffsetY(0), mWindowWidth(windowWidth), mWindowHeight(windowHeight), mMapWidth(mapWidth), mMapHeight(mapHeight)
    {
    }

    void Camera::update(float targetX, float targetY)
    {
        // Center the camera on the target (player)
        float desiredOffsetX = targetX - mWindowWidth / 2.0f;
        float desiredOffsetY = targetY - mWindowHeight / 2.0f;

        // Clamp the camera position to prevent showing beyond map boundaries
        mOffsetX = std::clamp(desiredOffsetX, 0.0f, static_cast<float>(mMapWidth - mWindowWidth));
        mOffsetY = std::clamp(desiredOffsetY, 0.0f, static_cast<float>(mMapHeight - mWindowHeight));
    }

    void Camera::worldToScreen(float worldX, float worldY, float &screenX, float &screenY) const
    {
        screenX = worldX - mOffsetX;
        screenY = worldY - mOffsetY;
    }

    void Camera::screenToWorld(float screenX, float screenY, float &worldX, float &worldY) const
    {
        worldX = screenX + mOffsetX;
        worldY = screenY + mOffsetY;
    }

} // namespace zuul