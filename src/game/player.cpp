#include <game/player.hpp>
#include <game/tilemap.hpp>
#include <SDL2/SDL.h>
#include <cmath>

namespace zuul
{

    Player::Player()
        : mTexture(nullptr),
          mTilesetData(::std::make_unique<TilesetData>()),
          mX(0), mY(0),
          mSpeed(200.0f),
          mWidth(32), mHeight(32),
          mDirection(Direction::Down),
          mCollisionBoxOffsetX(0), mCollisionBoxOffsetY(0),
          mCollisionBoxWidth(0), mCollisionBoxHeight(0)
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

        // Normalize diagonal movement
        if (dx != 0 && dy != 0)
        {
            const float normalizer = 1.0f / std::sqrt(2.0f);
            dx *= normalizer;
            dy *= normalizer;
        }

        // Calculate new position
        float newX = mX + dx * mSpeed * deltaTime;
        float newY = mY + dy * mSpeed * deltaTime;

        // Check collision at new position
        if (!tileMap.checkCollision(
                newX + mCollisionBoxOffsetX,
                newY + mCollisionBoxOffsetY,
                mCollisionBoxWidth,
                mCollisionBoxHeight))
        {
            mX = newX;
            mY = newY;
        }

        // Update animations
        mTilesetData->update(deltaTime);
    }

    void Player::render(std::shared_ptr<Renderer> renderer)
    {
        int baseTileId = static_cast<int>(mDirection);
        int currentTileId = mTilesetData->getCurrentTileId(baseTileId);
        int sourceX = currentTileId * mWidth;

        renderer->renderTexture(mTexture,
                                sourceX, 0, mWidth, mHeight,
                                static_cast<int>(mX), static_cast<int>(mY),
                                mWidth, mHeight);
    }

    void Player::renderDebug(std::shared_ptr<Renderer> renderer)
    {
        // Draw collision box in blue
        renderer->renderRect(
            static_cast<int>(mX + mCollisionBoxOffsetX),
            static_cast<int>(mY + mCollisionBoxOffsetY),
            static_cast<int>(mCollisionBoxWidth),
            static_cast<int>(mCollisionBoxHeight),
            0, 0, 255, 255 // Blue
        );
    }

} // namespace zuul