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
    int out;
    if ((out = walk_dir(add_stage, argv[i], NULL)) < 0) {
      return out;
    }
  }
  return 0;
}
