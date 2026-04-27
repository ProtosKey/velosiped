#include "utils/logger.h"
#include "utils/stager.h"
#include "utils/visitor.h"
#include "vls_command.h"
#include <dirent.h>
#include <errno.h>
#include <openssl/asn1.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int execute_add(const char *path, void *ctx) {
  int out;
  if ((out = add_stage(path)) < 0) {
    return out;
  }
  return 0;
}

int vls_add_func(const int argc, const char **argv) {
  if (argc < 1)
    return vls_report("Nothing specified, nothing added");

  for (int i = 0; i < argc; i++) {
    int out;
    if ((out = walk_dir(execute_add, argv[i], NULL)) < 0) {
      return out;
    }
  }
  return 0;
}
