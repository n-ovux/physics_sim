#pragma once
#include <stddef.h>
#include <stdint.h>

uint32_t createVAO(float vertices[], size_t verticesSize, uint32_t indices[], size_t indicesSize,
                   int numberOfAttributes, ...);
size_t glSizeOf(uint32_t type);
