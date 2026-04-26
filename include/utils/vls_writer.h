#ifndef VLH_WRITER_H
#define VLH_WRITER_H

#include <stddef.h>

typedef struct {
  const int descriptor;
  const char *message;
  const size_t size;
} vls_output_t;

int vls_write(vls_output_t out);

#endif
