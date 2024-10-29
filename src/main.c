#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cglm/types.h>
#include <cglm/vec2.h>
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
  float screenVertices[] = {
    -1, -1, 0, 0,
     1, -1, 1, 0,
    -1,  1, 0, 1,
     1,  1, 1, 1
  };

  uint32_t screenIndices[] = {
    0, 1, 2, 
    1, 2, 3
  };
  // clang-format on
  uint32_t screenVAO = createVAO(screenVertices, sizeof(screenVertices), screenIndices, sizeof(screenIndices), 2,
                                 GL_FLOAT, 2, GL_FLOAT, 2);

  uint32_t vertexShader = createShader("../src/shaders/default.vert", GL_VERTEX_SHADER);
  uint32_t fragmentShader = createShader("../src/shaders/default.frag", GL_FRAGMENT_SHADER);
  uint32_t screenShader = createProgram(2, vertexShader, fragmentShader);

  uint32_t screen;
  glGenTextures(1, &screen);
  glBindTexture(GL_TEXTURE_2D, screen);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_2D, 0);

  uint32_t renderShader = createShader("../src/shaders/render.comp", GL_COMPUTE_SHADER);
  uint32_t render = createProgram(1, renderShader);

  float radius = 5;
  int circleCount = 250;
  vec2 circlePositions[circleCount];
  for (int i = 0; i < circleCount; i++) {
    circlePositions[i][0] = rand() % WIDTH;
    circlePositions[i][1] = rand() % HEIGHT;
  }
  vec2 circlePositionsLast[circleCount];
  for (int i = 0; i < circleCount; i++)
    glm_vec2_copy(circlePositions[i], circlePositionsLast[i]);
  vec2 circlePositionsLastLast[circleCount];
  for (int i = 0; i < circleCount; i++)
    glm_vec2_copy(circlePositions[i], circlePositionsLastLast[i]);
  vec2 circleAccelerations[circleCount];
  for (int i = 0; i < circleCount; i++)
    glm_vec2_copy((vec2){0, -1000}, circleAccelerations[i]);

  glViewport(0, 0, WIDTH, HEIGHT);
  glEnable(GL_MULTISAMPLE);
  float currentTime = glfwGetTime();
  float lastTime = currentTime;
  while (!glfwWindowShouldClose(window)) {
    currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    printf("\rfps: %.3f  mspf: %.3f", 1.0f / deltaTime, deltaTime * 1000.0f);
    glClearColor(0, 0, 0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int subSteps = 8;
    float subDeltaTime = deltaTime / subSteps;
    for (int step = 0; step < subSteps; step++) {
      for (int i = 0; i < circleCount; i++) {
        circlePositions[i][0] = 2 * circlePositionsLast[i][0] - circlePositionsLastLast[i][0] +
                                circleAccelerations[i][0] * subDeltaTime * subDeltaTime;
        circlePositions[i][1] = 2 * circlePositionsLast[i][1] - circlePositionsLastLast[i][1] +
                                circleAccelerations[i][1] * subDeltaTime * subDeltaTime;
        if (circlePositions[i][1] < radius)
          circlePositions[i][1] = radius;
        if (circlePositions[i][1] > HEIGHT - radius)
          circlePositions[i][1] = HEIGHT - radius;
        if (circlePositions[i][0] < radius)
          circlePositions[i][0] = radius;
        if (circlePositions[i][0] > WIDTH - radius)
          circlePositions[i][0] = WIDTH - radius;

        vec2 temp = GLM_VEC2_ZERO_INIT;
        for (int j = 0; j < circleCount; j++) {
          glm_vec2_sub(circlePositions[i], circlePositions[j], temp);
          float distance = glm_vec2_norm(temp);
          if (distance < 2 * radius && distance != 0) {
            float delta = 2 * radius - distance;
            glm_vec2_scale(temp, 0.5f * delta * (1 / distance), temp);
            glm_vec2_add(circlePositions[i], temp, circlePositions[i]);
            glm_vec2_sub(circlePositions[j], temp, circlePositions[j]);
          }
        }
      }
      for (int i = 0; i < circleCount; i++)
        glm_vec2_copy(circlePositionsLast[i], circlePositionsLastLast[i]);
      for (int i = 0; i < circleCount; i++)
        glm_vec2_copy(circlePositions[i], circlePositionsLast[i]);
    }

    glUseProgram(render);
    glBindImageTexture(0, screen, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glUniform1f(glGetUniformLocation(render, "radius"), radius);
    glUniform2fv(glGetUniformLocation(render, "circles"), sizeof(circlePositions) / (sizeof(vec2)),
                 (float *)circlePositions);
    glDispatchCompute(WIDTH, HEIGHT, 1);
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glUseProgram(screenShader);
    glBindTexture(GL_TEXTURE_2D, screen);
    glBindVertexArray(screenVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}
