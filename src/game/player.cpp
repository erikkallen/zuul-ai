#include <game/player.hpp>
#include <SDL2/SDL.h>
#include <cmath>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace zuul
{

    Player::Player()
        : mX(0), mY(0), mSpeed(200.0f), mWidth(32), mHeight(32),
          mAnimationTimer(0), mDirection(Direction::Down)
    {
        // Initialize animation data with safe defaults
        mDirectionAnimations.resize(4); // One for each direction
        for (auto &anim : mDirectionAnimations)
        {
            anim.currentFrameIndex = 0;
        }
    }

    bool Player::initialize(::std::shared_ptr<Renderer> renderer)
    {
        mTexture = renderer->loadTexture("assets/player_tiles.png");
        if (!mTexture)
        {
            return false;
        }

        if (!loadTilesetData())
        {
            return false;
        }

        return true;
    }

    bool Player::loadTilesetData()
    {
        try
        {
            ::std::ifstream file("assets/player_tiles.tsj");
            if (!file.is_open())
            {
                ::std::cerr << "Failed to open player tileset file" << ::std::endl;
                return false;
            }

            json tilesetJson;
            file >> tilesetJson;

            // Process each tile's animation data
            const auto &tiles = tilesetJson["tiles"];
            for (const auto &tile : tiles)
            {
                int id = tile["id"].get<int>();
                if (tile.contains("animation"))
                {
                    int dirIndex = -1;

                    // Determine which direction this animation belongs to
                    if (id == 0)
                    {
                        dirIndex = static_cast<int>(Direction::Down);
                    }
                    else if (id == 3)
                    {
                        dirIndex = static_cast<int>(Direction::Up);
                    }
                    else if (id == 6)
                    {
                        dirIndex = static_cast<int>(Direction::Left);
                    }
                    else if (id == 9)
                    {
                        dirIndex = static_cast<int>(Direction::Right);
                    }
                    else
                        continue;

                    const auto &animation = tile["animation"];
                    auto &animData = mDirectionAnimations[dirIndex];
                    animData.frames.clear();

                    for (const auto &frame : animation)
                    {
                        AnimationFrame newFrame;
                        newFrame.tileId = frame["tileid"].get<int>();
                        newFrame.duration = frame["duration"].get<float>() / 1000.0f;
                        animData.frames.push_back(newFrame);
                    }
                    animData.currentFrameIndex = 0;
                }
            }

            return true;
        }
        catch (const ::std::exception &e)
        {
            ::std::cerr << "Error loading tileset data: " << e.what() << ::std::endl;
            return false;
        }
    }

    bool Player::tryMove(float newX, float newY, const TileMap &tileMap)
    {
        // Check if the new position would cause a collision
        if (!tileMap.checkCollision(newX, newY, mWidth, mHeight))
        {
            mX = newX;
            mY = newY;
            return true;
        }
        return false;
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
            const float normalizer = 1.0f / ::std::sqrt(2.0f);
            dx *= normalizer;
            dy *= normalizer;
        }

        // Calculate new position
        float newX = mX + dx * mSpeed * deltaTime;
        float newY = mY + dy * mSpeed * deltaTime;

        // Try to move to new position, checking for collisions
        bool moved = false;
        if (dx != 0 && dy != 0)
        {
            // For diagonal movement, try to slide along walls
            moved = tryMove(newX, newY, tileMap) ||
                    tryMove(mX + dx * mSpeed * deltaTime, mY, tileMap) ||
                    tryMove(mX, mY + dy * mSpeed * deltaTime, tileMap);
        }
        else
        {
            moved = tryMove(newX, newY, tileMap);
        }

        // Update animation
        updateAnimation(deltaTime, moved);
    }

    void Player::updateAnimation(float deltaTime, bool isMoving)
    {
        int dirIndex = static_cast<int>(mDirection);
        if (dirIndex >= 0 && dirIndex < static_cast<int>(mDirectionAnimations.size()))
        {
            auto &animData = mDirectionAnimations[dirIndex];

            if (isMoving && !animData.frames.empty())
            {
                mAnimationTimer += deltaTime;
                float currentFrameDuration = animData.frames[animData.currentFrameIndex].duration;

                if (mAnimationTimer >= currentFrameDuration)
                {
                    mAnimationTimer -= currentFrameDuration;
                    animData.currentFrameIndex = (animData.currentFrameIndex + 1) % animData.frames.size();
                }
            }
            else
            {
                // Reset to first frame when not moving
                animData.currentFrameIndex = 0;
                mAnimationTimer = 0;
            }
        }
    }

    int Player::getCurrentTileId() const
    {
        int dirIndex = static_cast<int>(mDirection);
        if (dirIndex >= 0 && dirIndex < static_cast<int>(mDirectionAnimations.size()))
        {
            const auto &animData = mDirectionAnimations[dirIndex];
            if (!animData.frames.empty())
            {
                return animData.frames[animData.currentFrameIndex].tileId;
            }
        }
        // Default to first frame of current direction if no animation data
        return static_cast<int>(mDirection) * 3;
    }

    void Player::render(::std::shared_ptr<Renderer> renderer)
    {
        int tileId = getCurrentTileId();
        int sourceX = tileId * mWidth;

        renderer->renderTexture(mTexture,
                                sourceX, 0, mWidth, mHeight,
                                static_cast<int>(mX), static_cast<int>(mY), mWidth, mHeight);
    }

} // namespace zuul