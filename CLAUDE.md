# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

Zuul is a tiling game remake written in C++23 using SDL3 and OpenGL. It's a recreation of an original Java game, featuring tile-based maps created with Tiled, animated sprites, shader effects, and a custom game engine.

## Build System

The project uses Meson build system with Ninja backend.

### Setup and Build

```bash
# Initial setup (from project root)
meson wrap install nlohmann_json
meson wrap install spdlog
mkdir build
cd build
meson setup ..
ninja

# Rebuild after changes
cd build
ninja
```

### Running the Game

```bash
# Must be run from the build directory
cd build
./zuul
```

The game requires `assets/` directory to be present in the working directory when running.

### Dependencies

- SDL3 (window/input/rendering)
- SDL3_image (image loading)
- SDL3_ttf (text rendering)
- OpenGL (graphics rendering, linked via `gl` dependency)
- nlohmann-json (JSON parsing, via Meson wrap)
- spdlog (logging, via Meson wrap)

**Note:** The project uses a custom OpenGL function loader (`gl_functions.hpp/cpp`) instead of GLEW to support Wayland. OpenGL 3.3+ core profile functions are loaded at runtime via `SDL_GL_GetProcAddress`.

## Code Architecture

### Engine Layer (`src/engine/`, `include/engine/`)

The engine provides core game framework functionality:

- **Game** - Abstract base class providing the main game loop with fixed timestep updates (60 FPS target). Handles SDL event polling and delegates to `update()` and `render()` methods. All games inherit from this.

- **Renderer** - Abstract rendering interface. Provides methods for texture loading, rendering textures/rectangles/text, and shader management.

- **SDLRenderer** - Concrete OpenGL-based renderer implementation. Uses OpenGL 3.3 core profile with vertex arrays, VBOs, and EBOs. Manages SDL window, OpenGL context, and integrates with the shader system.

- **gl_functions** - Custom OpenGL function loader that loads OpenGL 3.3+ core profile functions at runtime using SDL_GL_GetProcAddress. Wayland-compatible alternative to GLEW. Initializes all required OpenGL function pointers during renderer setup.

- **Shader & ShaderManager** - GLSL shader loading and management. ShaderManager is a singleton that caches loaded shaders. Shaders can set uniforms for time-based effects.

- **Texture** - Abstract texture interface with SDLTexture as OpenGL implementation using GL textures.

### Game Layer (`src/game/`, `include/game/`)

Game-specific implementation:

- **ZuulGame** - Main game class inheriting from `Game`. Manages title screen state, tilemap, player, camera, and UI. Coordinates game flow between title screen and gameplay.

- **TileMap** - Loads and renders Tiled JSON maps (`.tmj`). Supports multiple layers, tile animations (via TilesetData), collision detection, and items. Uses JSON for map data and tileset references.

- **TilesetData** - Parses Tiled tileset JSON (`.tsj`). Handles tile animations with frame durations, collision data per tile.

- **Player** - Character controller with sprite animation based on direction (Down/Up/Left/Right). Uses collision box for movement collision with tilemap. Loads collision data from JSON.

- **Camera** - Viewport management with zoom support. Tracks player position and calculates world-to-screen offsets for rendering.

- **UI** - Game UI rendering including inventory/collected items display.

- **TitleScreen** - Animated title screen that plays before the game starts.

- **Item** - Collectible items placed in the tilemap with collision detection.

### Data Flow

1. `main.cpp` creates `ZuulGame` and calls `initialize()` and `run()`
2. `Game::initialize()` creates `SDLRenderer`, initializes OpenGL, and loads shaders
3. `ZuulGame::initialize()` loads title screen, tilemap, player, camera, and UI
4. `Game::run()` runs the fixed timestep game loop:
   - Polls SDL events
   - Updates at fixed FRAME_TIME (1/60s) intervals
   - Renders as fast as possible
5. `ZuulGame::update()` handles title screen or gameplay state (keyboard input, player movement, camera tracking)
6. `ZuulGame::render()` renders title screen or game world (tilemap layers, player, items, UI)

### Rendering Pipeline

The renderer uses OpenGL with a quad-based sprite rendering system:
- All textures are rendered as textured quads
- Shaders can be applied per-texture for effects (water ripple, invert)
- Vertex shader receives position and texture coordinates
- Fragment shader applies effects and samples textures
- Shaders receive a `time` uniform for animated effects

### Shader System

Shaders are stored in `assets/shaders/` with `.vert` and `.frag` extensions:
- `water` - Animated water ripple effect with caustics
- `invert` - Color inversion effect

Shaders are loaded at engine initialization in `Game::initialize()` and accessed via `ShaderManager::getInstance().getShader(name)`.

### Map Data Format

Maps are created in Tiled and exported as JSON (`.tmj`):
- Multiple layers supported (background, foreground, etc.)
- Tile IDs reference tileset defined in `.tsj` files
- Collision data stored in tileset JSON custom properties
- Item placement via object layers

## Key Patterns

- Headers in `include/`, implementations in `src/`, mirroring the same directory structure
- Namespace `zuul` wraps all project code
- Smart pointers: `std::shared_ptr` for shared resources (Renderer, Texture), `std::unique_ptr` for owned objects
- Abstract interfaces (Renderer, Texture) with concrete implementations (SDLRenderer, SDLTexture)
- Singleton pattern for ShaderManager
- Fixed timestep game loop (update at 60 FPS, render as fast as possible)
- Callbacks for events (e.g., item collection callback from TileMap to UI)

## Debug Features

Press F3 in-game to toggle debug rendering mode, which shows collision boxes for tiles and entities.

## Common Modifications

- Add new shaders: Create `.vert` and `.frag` in `assets/shaders/`, load in `Game::initialize()`
- Add tilemap layers: Edit map in Tiled, layers are automatically loaded from JSON
- Modify player movement: Edit `Player::update()` in `src/game/player.cpp`
- Change collision detection: Modify `TileMap::checkCollision()` or adjust collision boxes in tileset JSON
