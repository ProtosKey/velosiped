#include "utils/input_output.h"
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

int vls_safety_copy(int in, int out) {
  char cp_buff[BUFFER_SIZE];
  ssize_t bytes_read;

  while (true) {
    bytes_read = read(in, cp_buff, BUFFER_SIZE);
    if (bytes_read < 0) {
      goto copy_error;
    } else if (bytes_read == 0) {
      break;
    }
    if (vls_safety_write((vls_output_t){out, cp_buff, bytes_read}) < 0) {
      return -1;
    }
  }

  return 0;
copy_error:
  const char *msg = strerror(errno);
  vls_safety_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
  return -1;
}
