project('physics_sim', 'c')

glew_dep = dependency('glew')
glfw3_dep = dependency('glfw3')
cglm_dep = dependency('cglm')

executable('physics_sim', './src/main.c', './src/shader.c', dependencies: [glew_dep, glfw3_dep, cglm_dep])
