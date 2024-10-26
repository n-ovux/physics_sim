project('physics_sim', 'c')

glew_dep = dependency('glew')
glfw3_dep = dependency('glfw3')
cglm_dep = dependency('cglm')
pthreads_dep = dependency('threads')

executable('physics_sim', './src/main.c', './src/shader.c', './src/vertex.c', './src/logging.c', dependencies: [glew_dep, glfw3_dep, cglm_dep, pthreads_dep])
