#include "utils/error.h"
#include "utils/md_hasher.h"
#include "vls_command.h"
#include <dirent.h>
#include <openssl/asn1.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int check_file_type(const char *name);
int copy_file(const char *name);
int parse_dir(const char *current);

int vls_add_func(const int argc, const char **argv) {
  if (argc < 1)
    return vls_report("Nothing specified, nothing added");

  for (int i = 0; i < argc; i++) {
    if (check_file_type(argv[i]) < 0)
      return -1;
  }
  return 0;
}
