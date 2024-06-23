project('physics_sim', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

glew_dep = dependency('glew')
glfw3_dep = dependency('glfw3')
cglm_dep = dependency('cglm')

executable('physics_sim', './src/main.c', dependencies: [glew_dep, glfw3_dep, cglm_dep])
