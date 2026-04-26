#include "utils/vls_writer.h"
#include "vls_command.h"
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int vls_add_func(const int argc, const char **argv) {
  const char *no_argv_msg = "Nothing specified, nothing added\n";
  if (argc < 1) {
    vls_safety_write(
        (vls_output_t){STDERR_FILENO, no_argv_msg, strlen(no_argv_msg)});
    return -1;
  }

  const char *no_add = "Not a file, not a directory\n";
  const char *no_such_file = "No such file or directory\n";
  for (int i = 0; i < argc; i++) {
    struct stat file;
    if (stat(argv[i], &file) == -1) {
      vls_safety_write(
          (vls_output_t){STDERR_FILENO, no_such_file, strlen(no_such_file)});
      return -1;
    }

    if (S_ISREG(file.st_mode)) {

    } else if (S_ISDIR(file.st_mode)) {

    } else {
      vls_safety_write((vls_output_t){STDERR_FILENO, no_add, strlen(no_add)});
      return -1;
    }
  }

  return 0;
}

int copy_file(char *source, char *destination) { return 0; }

int parse_dir(char *current) { return 0; }
