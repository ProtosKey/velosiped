#include "utils/vls_writer.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

int vls_write_all(vls_output_t out) {
  size_t count = 0;
  ssize_t n;

  if (out.size == 0) {
    return 0;
  }

  while (count < out.size) {
    n = write(out.descriptor, out.message + count, out.size - count);
    if (n == -1) {
      if (errno == EINTR) {
        continue;
      }
      return errno;
    } else if (n == 0) {
      return errno;
    }
    count += n;
  }

  return 0;
}
