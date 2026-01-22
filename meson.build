project('zuul', 'cpp', default_options: ['cpp_std=c++23'])

base_dir = meson.current_source_dir()
incdir = include_directories('include')

sdl3_dep = dependency('sdl3')
sdl3_image_dep = dependency('SDL3_image')
sdl3_ttf_dep = dependency('SDL3_ttf')
gl_dep = dependency('gl')
cpp = meson.get_compiler('cpp')
m_dep = cpp.find_library('m', required: true)
valgrind = find_program('valgrind', required: false)
spdlog = subproject('spdlog')
json = subproject('nlohmann_json')

spdlog_dep = spdlog.get_variable('spdlog_dep')
json_dep = json.get_variable('nlohmann_json_dep')

deps = [sdl3_dep, sdl3_image_dep, sdl3_ttf_dep, gl_dep, m_dep, spdlog_dep, json_dep]

sources = files(
    'src/engine/game.cpp',
    'src/engine/gl_functions.cpp',
    'src/engine/renderer.cpp',
    'src/engine/sdl_renderer.cpp',
    'src/engine/shader.cpp',
    'src/game/camera.cpp',
    'src/game/item.cpp',
    'src/game/player.cpp',
    'src/game/tilemap.cpp',
    'src/game/tileset_data.cpp',
    'src/game/title_screen.cpp',
    'src/game/ui.cpp',
    'src/game/zuul_game.cpp',
    'src/main.cpp',
)

executable(
    'zuul',
    sources,
    include_directories: incdir,
    dependencies: deps,
    cpp_args: ['-DLOG_USE_COLOR'],
)