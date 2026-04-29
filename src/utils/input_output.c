#include "utils/input_output.h"
#include "utils/logger.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>

int vls_safety_write(vls_output_t out) {
  size_t count = 0;
  ssize_t n;
  while (count < out.size) {
    n = write(out.fd, out.message + count, out.size - count);
    if (n == -1) {
      if (errno == EINTR) {
        continue;
      }
      return vls_report_errno(errno);
    } else if (n == 0) {
      return vls_report("write returned 0");
    }
    count += n;
  }
  return 0;
}

int vls_safety_copy(int in, int out) {
  char cp_buff[BUFFER_SIZE];
  while (true) {
    ssize_t bytes_read = read(in, cp_buff, BUFFER_SIZE);
    if (bytes_read < 0)
      return vls_report_errno(errno);
    if (bytes_read == 0)
      break;
    if (vls_safety_write((vls_output_t){out, cp_buff, bytes_read}) < 0)
      return -1;
  }
  return 0;
}
