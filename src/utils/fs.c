#include "utils/input_output.h"
#include "utils/logger.h"
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/cdefs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

__attribute__((nonnull(1), warn_unused_result())) int
vls_ensure_dir(const char *path) {
  if (mkdir(path, 0755) < 0 && errno != EEXIST) {
    return vls_report_errno_at(path, errno);
  }
  return 0;
}

__attribute__((nonnull(1), warn_unused_result(), const)) int
vls_ensure_file(const char *path, int oflag, ...) {
  int mod = 0;
  if (oflag & O_CREAT) {
    va_list list;
    va_start(list, oflag);
    mod = va_arg(list, int);
    va_end(list);
  }

  int fd = -1;
  if ((fd = open(path, oflag, mod)) < 0) {
    return vls_report_errno_at(path, errno);
  }
  return fd;
}

int vls_join_path(char *out, size_t cap, const char *dir, const char *name) {
  size_t dlen = strlen(dir);
  const char *sep = (dlen > 0 && dir[dlen - 1] != '/') ? "/" : "";
  int n = snprintf(out, cap, "%s%s%s", dir, sep, name);
  if (n < 0 || (size_t)n >= cap) {
    errno = ENAMETOOLONG;
    return -1;
  }
  return 0;
}

int vls_copy_file(const char *src, const char *dst, int dst_oflag) {
  int src_fd = vls_ensure_file(src, O_RDONLY);
  int dst_fd = vls_ensure_file(dst, dst_oflag, 0644);

  char cp_buff[BUFFER_SIZE];
  int result =
      (src_fd == -1 || dst_fd == -1 || vls_safety_copy(src_fd, dst_fd) < 0);

  close(src_fd);
  close(dst_fd);
  return result;
}
