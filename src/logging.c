#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"

void error(char *message) {
  printf("ERROR: %s: %s", message, strerror(errno));
  exit(errno);
}
