#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

#ifndef WRITER_H
#define WRITER_H

#include <stddef.h>

typedef struct {
  const int fd;
  const char *message;
  size_t size;
} vls_output_t;

int vls_safety_copy(int in, int out);
int vls_safety_write(vls_output_t out);

#endif
