#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cglm/cam.h>
#include <cglm/cglm.h>
#include <cglm/mat4.h>
#include <cglm/util.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>

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

  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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
  glm_perspective(M_PI / 3, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, projection);

  vec3 cameraPosition = {0.0f, 0.0f, 0.0f};
  float cameraPitch = 0.0f;
  float cameraYaw = 0.0f;

  double xpos, ypos = 0.0;
  glfwGetCursorPos(window, &xpos, &ypos);
  vec2 cursorLastPosition = {xpos, ypos};

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

    glfwGetCursorPos(window, &xpos, &ypos);
    vec2 cursorPosition = {xpos, ypos};
    vec2 cursorOffset = GLM_VEC2_ZERO_INIT;
    glm_vec2_sub(cursorPosition, cursorLastPosition, cursorOffset);
    glm_vec2_copy(cursorPosition, cursorLastPosition);
    glm_vec2_scale(cursorOffset, deltaTime * 10.0f, cursorOffset);
    cursorOffset[1] *= -1;

    cameraYaw += cursorOffset[0];
    cameraPitch += cursorOffset[1];
    if (cameraPitch > 89.0f)
      cameraPitch = 89.0f;
    if (cameraPitch < -89.0f)
      cameraPitch = -89.0f;

    vec3 up = {0.0f, 1.0f, 0.0f};
    vec3 front = GLM_VEC3_ZERO_INIT;
    front[0] = cos(glm_rad(cameraYaw)) * cos(glm_rad(cameraPitch));
    front[1] = sin(glm_rad(cameraPitch));
    front[2] = sin(glm_rad(cameraYaw)) * cos(glm_rad(cameraPitch));
    glm_vec3_normalize(front);

    float sensitivity = deltaTime * 5.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      vec3 temp;
      glm_vec3_copy(front, temp);
      glm_vec3_scale(temp, sensitivity, temp);
      glm_vec3_add(cameraPosition, temp, cameraPosition);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      vec3 temp;
      glm_vec3_copy(front, temp);
      glm_vec3_scale(temp, sensitivity, temp);
      glm_vec3_sub(cameraPosition, temp, cameraPosition);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      vec3 temp = GLM_VEC3_ZERO_INIT;
      glm_vec3_crossn(front, up, temp);
      glm_vec3_scale(temp, sensitivity, temp);
      glm_vec3_sub(cameraPosition, temp, cameraPosition);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      vec3 temp = GLM_VEC3_ZERO_INIT;
      glm_vec3_crossn(front, up, temp);
      glm_vec3_scale(temp, sensitivity, temp);
      glm_vec3_add(cameraPosition, temp, cameraPosition);
    }

    glm_vec3_add(front, cameraPosition, front);
    glm_lookat(cameraPosition, front, up, view);

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
