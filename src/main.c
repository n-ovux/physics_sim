#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 900
#define HEIGHT 600

GLFWwindow *window;

int main(void) {

  if (!glfwInit())
    return -1;

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
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwShowWindow(window);

  GLenum err = glewInit();
  if (err != GLEW_OK) {
    printf("Failed to initialize glew: %s\n", glewGetErrorString(err));
    glfwTerminate();
    exit(1);
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

  const char *vertexShaderSource = "#version 330 core\n"
                                   "layout (location = 0) in vec2 aPos;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   gl_Position = vec4(aPos, 0.0, 1.0);\n"
                                   "}\0";
  uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  int success;
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    printf("failed to compile vertex shader: %s", infoLog);
  }

  const char *fragmentShaderSource =
      "#version 330 core\n"
      "out vec4 FragColor;\n"
      "void main()\n"
      "{\n"
      "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
      "}\0";
  uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    printf("failed to compile fragment shader: %s", infoLog);
  }

  uint32_t shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    printf("failed to link shader program: %s", infoLog);
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

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
