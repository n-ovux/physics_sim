project('physics_sim', 'c')

cc = meson.get_compiler('c')
dependencies = [dependency('glew'), dependency('glfw3'), dependency('cglm'), dependency('threads'), cc.find_library('m')]

sources = ['./src/main.c', './src/shader.c', './src/vao.c', './src/logging.c']

executable('physics_sim', sources, dependencies: dependencies)
