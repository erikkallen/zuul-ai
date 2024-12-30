# Zuul remastered

Simple tiling game originally built in java by me and a classmate.
Now I am trying to recreate it as best I can in C++ using SDL just for fun using every AI tool possible!

![Screenshot of zuul](/assets/screenshot.png)

## Building for linux

### Dependencies

- SDL2
- SDL2 image
- SDL2 ttf
- Meson
- nlohmann-json
- spdlog

Ubuntu```apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev meson git```

Arch
```yay -S sdl2 sdl2_image sdl2_ttf meson``` 

```bash
git clone https://github.com/erikkallen/zuul-remastered.git
cd zuul
meson wrap add nlohmann-json
meson wrap add spdlog
mkdir build
cd build
meson setup ..
ninja
```

## Running

```bash
# Copy the assets.json to the directory you built the projects e.g. <project_root>/build
cp assets/*.{tmj,png,ttf,tsj} build/
# Make sure you are in the directory where you built the project
./zuul
```

## Testing

```bash
meson test
```

## Map making

For mapmaking I used Tiled. Currently the following features are supported in the engine:

- Multiple layers
- Animations using the tiled animation editor


## Thanks to the following projects for their awesome tools/libraries/inspiration

- [Tiled](https://www.mapeditor.org/)
- [Pixelorama](https://github.com/Orama-Interactive/Pixelorama)
- [SDL](https://www.libsdl.org/)
- [Kenny](https://www.kenney.nl/assets/roguelike-rpg-pack)
