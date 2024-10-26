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
  float cubeVertices[] = {
     -1, -1,  1, //0
      1, -1,  1, //1
     -1,  1,  1, //2
      1,  1,  1, //3
     -1, -1, -1, //4
      1, -1, -1, //5
     -1,  1, -1, //6
      1,  1, -1  //7
  };

  uint32_t cubeIndices[] = {
      //Top
      2, 6, 7,
      2, 3, 7,
      //Bottom
      0, 4, 5,
      0, 1, 5,
      //Left
      0, 2, 6,
      0, 4, 6,
      //Right
      1, 3, 7,
      1, 5, 7,
      //Front
      0, 2, 3,
      0, 1, 3,
      //Back
      4, 6, 7,
      4, 5, 7
  };
  float planeVertices[] = {
    -1, 0, -1,
     1, 0, -1,
     1, 0,  1,
    -1, 0,  1,
  };

  uint32_t planeIndices[] = {
    0, 1, 2, 
    0, 3, 2
  };
  // clang-format on

  uint32_t cubeVAO = createVAO(cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices), 1, GL_FLOAT, 3);
  uint32_t planeVAO =
      createVAO(planeVertices, sizeof(planeVertices), planeIndices, sizeof(planeIndices), 1, GL_FLOAT, 3);

  uint32_t vertexShader = createShader("../src/shaders/default.vert", GL_VERTEX_SHADER);
  uint32_t fragmentShader = createShader("../src/shaders/default.frag", GL_FRAGMENT_SHADER);
  uint32_t shader = createProgram(2, vertexShader, fragmentShader);

  mat4 cubeModel = GLM_MAT4_IDENTITY_INIT;
  mat4 planeModel = GLM_MAT4_IDENTITY_INIT;
  glm_translate(planeModel, (vec3){0, -1.001, 0});
  glm_scale(planeModel, (vec3){4, 0, 4});
  mat4 view = GLM_MAT4_IDENTITY_INIT;
  mat4 projection = GLM_MAT4_IDENTITY_INIT;
  glm_perspective(M_PI / 3, (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f, projection);

  vec3 cameraPosition = GLM_VEC3_ZERO_INIT;
  float cameraPitch = 0.0f;
  float cameraYaw = 0.0f;

  double xpos, ypos = 0.0;
  glfwGetCursorPos(window, &xpos, &ypos);
  vec2 cursorLastPosition = {xpos, ypos};

  glViewport(0, 0, WIDTH, HEIGHT);
  glEnable(GL_MULTISAMPLE);
  glEnable(GL_DEPTH_TEST);
  float currentTime = glfwGetTime();
  float lastTime = currentTime;
  while (!glfwWindowShouldClose(window)) {
    currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    printf("\rfps: %.3f  mspf: %.3f", 1.0f / deltaTime, deltaTime * 1000.0f);
    glClearColor(0.23f, 0.24f, 0.30f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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

    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glm_vec3_add(front, cameraPosition, front);
    glm_lookat(cameraPosition, front, up, view);

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uView"), 1, GL_FALSE, view[0]);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uProjection"), 1, GL_FALSE, projection[0]);

    glUniform3f(glGetUniformLocation(shader, "uColor"), 1.0f, 1.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, cubeModel[0]);
    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glUniform3f(glGetUniformLocation(shader, "uColor"), 0.5f, 0.5f, 0.5f);
    glUniformMatrix4fv(glGetUniformLocation(shader, "uModel"), 1, GL_FALSE, planeModel[0]);
    glBindVertexArray(planeVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
