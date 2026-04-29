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

static void prepare_error() {
  write_stderr(CLR_RED, strlen(CLR_RED));
  write_stderr("[Error]\t", 9);
  write_stderr(CLR_RESET, strlen(CLR_RESET));
}

int vls_raw(const char *msg) {
  write_stdout(msg, strlen(msg));
  return 0;
}

int vls_raw_green(const char *msg) {
  write_stdout(CLR_BOLD, strlen(CLR_BOLD));
  write_stdout(CLR_GREEN, strlen(CLR_GREEN));
  write_stdout(msg, strlen(msg));
  write_stdout(CLR_RESET, strlen(CLR_RESET));
  return 0;
}

int vls_say(const char *msg) {
  write_stdout(msg, strlen(msg));
  write_stdout("\n", 1);
  return 0;
}

int vls_say_green(const char *msg) {
  write_stdout(CLR_BOLD, strlen(CLR_BOLD));
  write_stdout(CLR_GREEN, strlen(CLR_GREEN));
  write_stdout(msg, strlen(msg));
  write_stdout("\n", 1);
  write_stdout(CLR_RESET, strlen(CLR_RESET));
  return 0;
}

static int vls_report_no_label(const char *msg) {
  write_stderr(msg, strlen(msg));
  write_stderr("\n", 1);
  return -1;
}

int vls_report(const char *msg) {
  prepare_error();
  return vls_report_no_label(msg);
}

int vls_report_at(const char *ctx, const char *msg) {
  prepare_error();
  write_stderr(ctx, strlen(ctx));
  write_stderr(": ", 2);
  return vls_report_no_label(msg);
}

int vls_report_errno(int err) {
  prepare_error();
  return vls_report_no_label(strerror(err));
}

int vls_report_errno_at(const char *ctx, int err) {
  prepare_error();
  write_stderr(ctx, strlen(ctx));
  write_stderr(": ", 2);
  return vls_report_no_label(strerror(err));
}
