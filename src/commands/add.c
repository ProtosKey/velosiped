#include "utils/input_output.h"
#include "utils/md_hasher.h"
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

int vls_add_func(const int argc, const char **argv) {
  const char *no_argv_msg = "Nothing specified, nothing added\n";
  if (argc < 1) {
    vls_safety_write(
        (vls_output_t){STDERR_FILENO, no_argv_msg, strlen(no_argv_msg)});
    return -1;
  }
  for (int i = 0; i < argc; i++) {
    if (check_file_type(argv[i]) < 0) {
      return -1;
    }
  }
  return 0;
}

int check_file_type(const char *name) {
  if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
    return 0;
  }
  struct stat file;
  const char *no_add = "Not a file, not a directory\n";
  if (stat(name, &file) < 0) {
    const char *msg = strerror(errno);
    vls_safety_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
    return -1;
  }

  if (S_ISREG(file.st_mode)) {
    if (copy_file(name) < 0) {
      return -1;
    }
  } else if (S_ISDIR(file.st_mode)) {
    if (parse_dir(name) < 0) {
      return -1;
    }
  } else {
    vls_safety_write((vls_output_t){STDERR_FILENO, no_add, strlen(no_add)});
    return -1;
  }

  return 0;
}

int copy_file(const char *name) {
  vls_md_hash_t result;
  if (hash_my_path(name, &result) < 0) {
    return -1;
  }
  if (mkdir(STAGE_DIR, 0755) < 0) {
    if (errno != EEXIST) {
      goto error_create;
    }
  } // TODO (запись + отслеживание)
  return 0;
error_create:
  const int err = errno;
  const char *msg = strerror(errno);
  vls_safety_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
  return -1;
}

int parse_dir(const char *current) {
  DIR *dir_open;
  struct dirent *dir;
  dir_open = opendir(current);
  if (dir_open) {
    while ((dir = readdir(dir_open)) != NULL) {
      if (check_file_type(dir->d_name) < 0) {
        return -1;
      }
    }
  } else {
    goto error_check_dir;
  }
  return 0;
error_check_dir:
  const int err = errno;
  const char *msg = strerror(errno);
  vls_safety_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
  return -1;
}
