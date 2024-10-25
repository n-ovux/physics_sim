#include <GL/glew.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "shader.h"

uint32_t createShader(char *sourceFile, uint32_t type) {
  FILE *file;
  file = fopen(sourceFile, "r");
  if (file == NULL) {
    err("Fail to open file");
  }

  char *source = calloc(256, sizeof(char));
  char line[256];
  while (fgets(line, 256, file) != NULL) {
    strcat(source, line);
  }
  uint32_t shader = glCreateShader(type);
  glShaderSource(shader, 1, (const char *const *)&source, NULL);
  glCompileShader(shader);
  free(source);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    err(infoLog);
  }
  return shader;
}

uint32_t createProgram(int numberOfShaders, ...) {
  uint32_t program = glCreateProgram();
  va_list shaders;

  va_start(shaders, numberOfShaders);
  for (int i = 0; i < numberOfShaders; i++) {
    glAttachShader(program, va_arg(shaders, uint32_t));
  }
  va_end(shaders);

  int success;
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    err(infoLog);
  }

  va_start(shaders, numberOfShaders);
  for (int i = 0; i < numberOfShaders; i++)
    glDeleteShader(va_arg(shaders, uint32_t));
  va_end(shaders);

  return program;
}
