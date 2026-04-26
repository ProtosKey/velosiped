#include "utils/dir_walker.h"
#include "utils/input_output.h"
#include "utils/md_hasher.h"
#include "utils/stage_reader.h"
#include "vls_command.h"
#include <dirent.h>
#include <errno.h>
#include <openssl/asn1.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int check_file_type(const char *name);
int copy_file(const char *name);
int parse_dir(const char *current);

int stage_file(const char *path, void *ctx) {
  vls_md_hash_t result;
  if (hash_my_path(path, &result) < 0) {
    return -1;
  }
  if (mkdir(STAGE_DIR, 0755) < 0) {
    if (errno != EEXIST) {
      goto error_create;
    }
  }
  if (add_stage(&(stage){path, &result}) < 0) {
    return -1;
  }
  return 0;
error_create:
  const int err = errno;
  const char *msg = strerror(errno);
  vls_safety_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
  return -1;
}

int vls_add_func(const int argc, const char **argv) {
  const char *no_argv_msg = "Nothing specified, nothing added\n";
  if (argc < 1) {
    vls_safety_write(
        (vls_output_t){STDERR_FILENO, no_argv_msg, strlen(no_argv_msg)});
    return -1;
  }
  for (int i = 0; i < argc; i++) {
    walk_dir(*stage_file, argv[i], NULL);
  }
  return 0;
}
