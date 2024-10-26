#pragma once
#include <stddef.h>
#include <stdint.h>

uint32_t createVAO(float vertices[], size_t size, int numberOfAttributes, ...);
size_t glSizeOf(uint32_t type);
