#include "utils/logger.h"
#include "utils/input_output.h"
#include <string.h>
#include <unistd.h>

static void write_stderr(const char *s, size_t n) {
  vls_safety_write((vls_output_t){STDERR_FILENO, s, n});
}

static void write_stdout(const char *s, size_t n) {
  vls_safety_write((vls_output_t){STDOUT_FILENO, s, n});
}

int vls_say(const char *msg) {
  write_stdout(msg, strlen(msg));
  write_stdout("\n", 1);
  return 0;
}

int vls_report(const char *msg) {
  write_stderr(msg, strlen(msg));
  write_stderr("\n", 1);
  return -1;
}

int vls_report_at(const char *ctx, const char *msg) {
  write_stderr(ctx, strlen(ctx));
  write_stderr(": ", 2);
  return vls_report(msg);
}

int vls_report_errno(int err) { return vls_report(strerror(err)); }

int vls_report_errno_at(const char *ctx, int err) {
  write_stderr(ctx, strlen(ctx));
  write_stderr(": ", 2);
  return vls_report(strerror(err));
}
