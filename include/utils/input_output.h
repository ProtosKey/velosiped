#ifndef WRITER_H
#define WRITER_H
#define BUFFER_SIZE 4096

#include <stddef.h>

typedef struct {
  const int fd;
  const char *message;
  const size_t size;
} vls_output_t;

int vls_safety_copy(int in, int out);
int vls_safety_write(vls_output_t out);

#endif
