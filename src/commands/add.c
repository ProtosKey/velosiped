#include "utils/hasher.h"
#include "utils/logger.h"
#include "utils/visitor.h"
#include "vls_command.h"
#include <dirent.h>
#include <errno.h>
#include <openssl/asn1.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int add_stage(const char *path, void *ctx) { return 0; }

int vls_add_func(const int argc, const char **argv) {
  if (argc < 1)
    return vls_report("Nothing specified, nothing added");

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "..") == 0) {
      continue;
    }
    struct stat file;
    if (stat(argv[i], &file) < 0) {
      return vls_report_errno(errno);
    }
    if (S_ISREG(file.st_mode)) {
      int out = 0;
      if ((out = add_stage(argv[i], NULL)) < 0)
        return out;
    } else if (S_ISDIR(file.st_mode)) {
      int out = 0;
      if ((out = walk_dir(add_stage, argv[i], NULL)) < 0) {
        return out;
      }
    } else {
      return vls_report("Not a file, not a directory");
    }
  }
  return 0;
}
