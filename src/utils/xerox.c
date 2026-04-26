#include "utils/error_logger.h"
#include "utils/input_output.h"
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

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
