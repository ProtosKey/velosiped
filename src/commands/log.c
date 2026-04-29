#include "utils/fs.h"
#include "utils/input_output.h"
#include "utils/logger.h"
#include "utils/time.h"
#include "vls_command.h"
#include "vls_paths.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define HASH_LEN 32

#define VLS_FILES(X)                                                           \
  X(msg)                                                                       \
  X(prev)                                                                      \
  X(time)

static int print_commit(const char *hash_str, char prev_out[HASH_LEN + 1]) {
  char path[PATH_MAX];
  if (vls_join_path(path, PATH_MAX, VLS_COMMITS_DIR, hash_str) < 0)
    return -1;

  int cmt_fd = open(path, O_RDONLY | O_DIRECTORY);
  if (cmt_fd < 0)
    return vls_report_errno_at(hash_str, errno);

  int rc = 0;

#define OPEN(file) int file##_fd = openat(cmt_fd, #file, O_RDONLY);
  VLS_FILES(OPEN)
#undef OPEN
#define CHECK(file)                                                            \
  if (file##_fd < 0) {                                                         \
    rc = vls_report_errno_at(#file, errno);                                    \
    goto cleanup;                                                              \
  }
  VLS_FILES(CHECK)
#undef CHECK

  char msg[BUFFER_SIZE];
  ssize_t mn = read(msg_fd, msg, sizeof msg - 1);
  if (mn < 0) {
    rc = vls_report_errno(errno);
    goto cleanup;
  }
  msg[mn] = '\0';

  ssize_t pn = read(prev_fd, prev_out, HASH_LEN);
  if (pn < 0) {
    rc = vls_report_errno(errno);
    goto cleanup;
  }
  prev_out[pn] = '\0';

  const time_t created = parse_from_fd(time_fd);

  vls_raw("Commit:\t");
  vls_say_green(hash_str);
  vls_raw("Date:\t");
  vls_say(ctime(&created));
  vls_raw("Name:\t");
  vls_raw(CLR_CYAN);
  vls_raw(CLR_BOLD);
  vls_say(msg);
  vls_say(CLR_RESET);

cleanup:
#define CLOSE(file)                                                            \
  if (file##_fd >= 0)                                                          \
    close(file##_fd);
  VLS_FILES(CLOSE)
#undef CLOSE

  close(cmt_fd);
  return rc;
}

int vls_log_func(const int argc, const char **argv) {
  int fd = open(VLS_HEAD_FILE, O_RDONLY);
  if (fd < 0)
    return vls_report_errno_at(VLS_HEAD_FILE, errno);

  char hash[HASH_LEN + 1] = {0};
  ssize_t n = read(fd, hash, HASH_LEN);
  close(fd);

  if (n == 0)
    return vls_report("nothing committed yet");
  if (n != HASH_LEN)
    return vls_report("HEAD file is corrupt");
  hash[n] = '\0';

  while (hash[0] != '\0') {
    char prev[HASH_LEN + 1] = {0};
    if (print_commit(hash, prev) < 0)
      return -1;
    memcpy(hash, prev, sizeof hash);
  }
  return 0;
}
