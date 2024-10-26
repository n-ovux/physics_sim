#include <GL/glew.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "vertex.h"

uint32_t createVAO(float vertices[], size_t size, int numberOfAttributes, ...) {
  uint32_t VAO, VBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);

  va_list types;
  size_t totalSize = 0;
  va_start(types, numberOfAttributes);
  for (int i = 0; i < numberOfAttributes; i++) {
    uint32_t type = va_arg(types, uint32_t);
    int amount = va_arg(types, int);
    totalSize += amount * glSizeOf(type);
  }
  va_end(types);

  size_t partialSize = 0;
  va_start(types, numberOfAttributes);
  for (int i = 0; i < numberOfAttributes; i++) {
    uint32_t type = va_arg(types, uint32_t);
    int amount = va_arg(types, int);
    glVertexAttribPointer(i, amount, type, GL_FALSE, totalSize,
                          (void *)partialSize);
    glEnableVertexAttribArray(i);
    partialSize += glSizeOf(type);
  }
  va_end(types);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return VAO;
}

size_t glSizeOf(uint32_t type) {
  switch (type) {
  case GL_FLOAT:
    return sizeof(float);
  case GL_INT:
    return sizeof(int);
  default:
    err("failed to find type");
  }
}
