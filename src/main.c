#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "shader.h"

#define WIDTH 900
#define HEIGHT 600

GLFWwindow *window;

int main(void) {

  if (!glfwInit()) {
    err("failed to initialize glfw");
  }

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 4);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Physics Sim", NULL, NULL);
  if (!window) {
    glfwTerminate();
    err("Failed to make window");
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwShowWindow(window);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    err("Failed to initialize glew");
  }

  float vertices[] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.0f, 0.5f};

  uint32_t VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  uint32_t vertexShader =
      createShader("../src/shaders/default.vert", GL_VERTEX_SHADER);
  uint32_t fragmentShader =
      createShader("../src/shaders/default.frag", GL_FRAGMENT_SHADER);
  uint32_t shaderProgram = createProgram(2, vertexShader, fragmentShader);

  glViewport(0, 0, WIDTH, HEIGHT);
  glEnable(GL_MULTISAMPLE);
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.23f, 0.24f, 0.30f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
