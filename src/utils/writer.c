#include "utils/error_logger.h"
#include "utils/input_output.h"
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
      return vls_report_errno(errno);
    }
    count += n;
  }
  return 0;
}
