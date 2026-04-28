#include "utils/time.h"
#include "utils/input_output.h"
#include "vls_types.h"

int write_time_to_vls(int fd) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  char buf[TIME_BUF_SIZE];

  size_t len = strftime(buf, sizeof(buf), FMT, t);
  if (len == 0)
    return -1;

  vls_output_t out = {.fd = fd, .message = buf, .size = len};

  return vls_safety_write(out);
}

time_t parse_from_fd(int fd) {
  char buf[TIME_BUF_SIZE];
  ssize_t n = read(fd, buf, sizeof(buf) - 1);
  if (n <= 0)
    return (time_t)-1;

  buf[n] = '\0';

  struct tm t = {0};
  if (strptime(buf, FMT, &t) == NULL) {
    return (time_t)-1;
  }

  t.tm_isdst = -1;
  return mktime(&t);
}
