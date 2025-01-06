project('zuul', 'cpp', default_options: ['cpp_std=c++23'])

base_dir = meson.current_source_dir()
incdir = include_directories('include')

sdl2_dep = dependency('sdl2')
sdl2_image_dep = dependency('SDL2_image')
sdl2_ttf_dep = dependency('SDL2_ttf')
cpp = meson.get_compiler('cpp')
m_dep = cpp.find_library('m', required: true)
valgrind = find_program('valgrind', required: false)
spdlog = subproject('spdlog')
json = subproject('nlohmann_json')

spdlog_dep = spdlog.get_variable('spdlog_dep')
json_dep = json.get_variable('nlohmann_json_dep')

deps = [sdl2_dep, sdl2_image_dep, sdl2_ttf_dep, m_dep, spdlog_dep, json_dep]

sources = files(
    'src/main.cpp',
    'src/engine/game.cpp',
    'src/engine/sdl_renderer.cpp',
    'src/engine/renderer.cpp',
    'src/game/tilemap.cpp',
    'src/game/tileset_data.cpp',
    'src/game/zuul_game.cpp',
    'src/game/player.cpp',
    'src/game/camera.cpp',
    'src/game/item.cpp',
    'src/game/ui.cpp',
)

executable(
    'zuul',
    sources,
    include_directories: incdir,
    dependencies: deps,
    cpp_args: ['-DLOG_USE_COLOR'],
)
