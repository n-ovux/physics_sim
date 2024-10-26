#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cam.h>
#include <cglm/cglm.h>
#include <cglm/mat4.h>

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "shader.h"
#include "vao.h"

#define WIDTH 1600
#define HEIGHT 900

int main(void) {
  GLFWwindow *window;
  if (!glfwInit())
    error("failed to initialize glfw");

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Physics Sim", NULL, NULL);
  if (!window)
    error("Failed to make window");

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwShowWindow(window);

  GLenum err = glewInit();
  if (err != GLEW_OK)
    error("Failed to initialize glew");

  // clang-format off
  float vertices[] = {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 
                       0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 
                       0.0f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f};
  // clang-format on
  uint32_t VAO = createVAO(vertices, sizeof(vertices), 2, GL_FLOAT, 3, GL_FLOAT, 3);

  uint32_t vertexShader = createShader("../src/shaders/default.vert", GL_VERTEX_SHADER);
  uint32_t fragmentShader = createShader("../src/shaders/default.frag", GL_FRAGMENT_SHADER);
  uint32_t shader = createProgram(2, vertexShader, fragmentShader);

  mat4 model = GLM_MAT4_IDENTITY_INIT;
  mat4 view = GLM_MAT4_IDENTITY_INIT;
  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_scale_make(model, (vec3){1.0f, 1.0f, 1.0});
  glm_translate_make(view, (vec3){0.0f, 0.0f, -1.0f});
  glm_perspective(M_PI / 3, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, projection);

  glViewport(0, 0, WIDTH, HEIGHT);
  glEnable(GL_MULTISAMPLE);
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.23f, 0.24f, 0.30f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, model[0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uView"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"), 1, GL_FALSE, projection[0]);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
