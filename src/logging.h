#pragma once
#include <errno.h>

#define err(message)                                                           \
  printf("ERROR: %s: %s", message, strerror(errno));                           \
  exit(errno);
