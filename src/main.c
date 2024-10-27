#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "shader.h"
#include "vao.h"

#define WIDTH 1600
#define HEIGHT 900

void resizeCallback(GLFWwindow *window, int width, int height) { glViewport(0, 0, width, height); }

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  glfwSetFramebufferSizeCallback(window, resizeCallback);

  GLenum err = glewInit();
  if (err != GLEW_OK)
    error("Failed to initialize glew");

  // clang-format off
  float planeVertices[] = {
    -1, -1, 0, 0,
     1, -1, 1, 0,
    -1,  1, 0, 1,
     1,  1, 1, 1
  };

  uint32_t planeIndices[] = {
    0, 1, 2, 
    1, 2, 3
  };
  // clang-format on
  uint32_t planeVAO =
      createVAO(planeVertices, sizeof(planeVertices), planeIndices, sizeof(planeIndices), 2, GL_FLOAT, 2, GL_FLOAT, 2);

  uint32_t vertexShader = createShader("../src/shaders/default.vert", GL_VERTEX_SHADER);
  uint32_t fragmentShader = createShader("../src/shaders/default.frag", GL_FRAGMENT_SHADER);
  uint32_t shader = createProgram(2, vertexShader, fragmentShader);

  uint32_t texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  uint32_t renderShader = createShader("../src/shaders/render.comp", GL_COMPUTE_SHADER);
  uint32_t render = createProgram(1, renderShader);
  glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
  glUseProgram(render);
  glDispatchCompute(WIDTH, HEIGHT, 1);
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

  glViewport(0, 0, WIDTH, HEIGHT);
  glEnable(GL_MULTISAMPLE);
  float currentTime = glfwGetTime();
  float lastTime = currentTime;
  while (!glfwWindowShouldClose(window)) {
    currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    printf("\rfps: %.3f  mspf: %.3f", 1.0f / deltaTime, deltaTime * 1000.0f);
    glClearColor(0.23f, 0.24f, 0.30f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shader);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(planeVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
