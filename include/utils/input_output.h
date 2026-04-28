#ifndef WRITER_H
#define WRITER_H
#define BUFFER_SIZE 4096

#include "vls_types.h"
#include <stddef.h>

int vls_safety_copy(int in, int out);
int vls_safety_write(vls_output_t out);

#endif
