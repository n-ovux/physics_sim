#pragma once
#include <stdint.h>

uint32_t createShader(char *sourceFile, uint32_t type);
uint32_t createProgram(int numberOfShaders, ...);
