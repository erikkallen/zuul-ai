#include <SDL2/SDL.h>
#include <cmath>
#include <memory>
#include <engine/renderer.hpp>
#include <game/player.hpp>
#include <game/tilemap.hpp>

namespace zuul
{
    Player::Player()
        : mDirection(Direction::Down),
          mTilesetData(std::make_shared<TilesetData>()),
          mTexture(nullptr),
          mX(0),
          mY(0),
          mSpeed(200.0f),
          mWidth(32),
          mHeight(32),
          mTilesetColumns(3),
          mDebugRendering(false),
          mIsMoving(false),
          mCollisionBoxOffsetX(0),
          mCollisionBoxOffsetY(0),
          mCollisionBoxWidth(0),
          mCollisionBoxHeight(0)
    {
    }

    bool Player::initialize(std::shared_ptr<Renderer> renderer)
    {
        mTexture = renderer->loadTexture("assets/player_tiles.png");
        if (!mTexture)
        {
            return false;
        }

        if (!mTilesetData->loadFromFile("assets/player_tiles.tsj"))
        {
            return false;
        }

        // Get tileset grid information
        const auto &tilesetInfo = mTilesetData->getTilesetInfo();
        mTilesetColumns = tilesetInfo.columns;
        mWidth = tilesetInfo.tileWidth;
        mHeight = tilesetInfo.tileHeight;

        // Get collision box from tileset data
        if (const CollisionBox *box = mTilesetData->getCollisionBox(0))
        {
            mCollisionBoxOffsetX = box->x;
            mCollisionBoxOffsetY = box->y;
            mCollisionBoxWidth = box->width;
            mCollisionBoxHeight = box->height;
        }
        else
        {
            // Default collision box if none defined
            mCollisionBoxOffsetX = 4;
            mCollisionBoxOffsetY = 0;
            mCollisionBoxWidth = 24;
            mCollisionBoxHeight = 32;
        }

        return true;
    }

    void Player::update(float deltaTime, const TileMap &tileMap)
    {
        const uint8_t *keyState = SDL_GetKeyboardState(nullptr);

        float dx = 0;
        float dy = 0;

        if (keyState[SDL_SCANCODE_W] || keyState[SDL_SCANCODE_UP])
        {
            dy -= 1;
            mDirection = Direction::Up;
        }
        if (keyState[SDL_SCANCODE_S] || keyState[SDL_SCANCODE_DOWN])
        {
            dy += 1;
            mDirection = Direction::Down;
        }
        if (keyState[SDL_SCANCODE_A] || keyState[SDL_SCANCODE_LEFT])
        {
            dx -= 1;
            mDirection = Direction::Left;
        }
        if (keyState[SDL_SCANCODE_D] || keyState[SDL_SCANCODE_RIGHT])
        {
            dx += 1;
            mDirection = Direction::Right;
        }

        // Check if player is moving
        mIsMoving = (dx != 0 || dy != 0);

        // Normalize diagonal movement
        if (dx != 0 && dy != 0)
        {
            const float normalizer = 1.0f / std::sqrt(2.0f);
            dx *= normalizer;
            dy *= normalizer;
        }

        // Update collision box before movement checks
        updateCollisionBox();

        // Try X movement first
        float newX = mX + dx * mSpeed * deltaTime;
        if (!tileMap.checkCollision(
                newX + mCollisionBoxOffsetX,
                mY + mCollisionBoxOffsetY,
                mCollisionBoxWidth,
                mCollisionBoxHeight))
        {
            mX = newX;
        }

        // Then try Y movement
        float newY = mY + dy * mSpeed * deltaTime;
        if (!tileMap.checkCollision(
                mX + mCollisionBoxOffsetX,
                newY + mCollisionBoxOffsetY,
                mCollisionBoxWidth,
                mCollisionBoxHeight))
        {
            mY = newY;
        }

        // Only update animations if moving
        if (mIsMoving)
        {
            mTilesetData->update(deltaTime);
        }
    }

    void Player::updateCollisionBox()
    {
        int baseFrame = getBaseFrame();
        int currentTileId = mIsMoving ? mTilesetData->getCurrentTileId(baseFrame) : baseFrame;

        if (const CollisionBox *box = mTilesetData->getCollisionBox(currentTileId))
        {
            mCollisionBoxOffsetX = box->x;
            mCollisionBoxOffsetY = box->y;
            mCollisionBoxWidth = box->width;
            mCollisionBoxHeight = box->height;
        }
    }

    void Player::render(std::shared_ptr<Renderer> renderer, float offsetX, float offsetY, float zoom)
    {
        // Calculate screen position with zoom
        float screenX = std::floor((mX - offsetX) * zoom);
        float screenY = std::floor((mY - offsetY) * zoom);
        int destW = static_cast<int>(std::ceil(mWidth * zoom));
        int destH = static_cast<int>(std::ceil(mHeight * zoom));

        // Get current animation frame or base frame if not moving
        int currentTileId;
        if (mIsMoving)
        {
            currentTileId = mTilesetData->getCurrentTileId(getBaseFrame());
        }
        else
        {
            currentTileId = getBaseFrame(); // Use base frame when standing still
        }

        // Calculate source rectangle in tileset
        int srcX = (currentTileId % mTilesetColumns) * mWidth;
        int srcY = (currentTileId / mTilesetColumns) * mHeight;

        // Render the player sprite scaled by zoom
        renderer->renderTexture(mTexture,
                                srcX, srcY, mWidth, mHeight,
                                static_cast<int>(screenX),
                                static_cast<int>(screenY),
                                destW, destH);

        // Render collision box in debug mode
        if (mDebugRendering)
        {
            // Draw collision box in blue
            renderer->renderRect(
                static_cast<int>((mX + mCollisionBoxOffsetX - offsetX) * zoom),
                static_cast<int>((mY + mCollisionBoxOffsetY - offsetY) * zoom),
                static_cast<int>(mCollisionBoxWidth * zoom),
                static_cast<int>(mCollisionBoxHeight * zoom),
                0, 0, 255, 255 // Blue
            );
        }
    }

    int Player::getBaseFrame() const
    {
        return static_cast<int>(mDirection);
    }

} // namespace zuul