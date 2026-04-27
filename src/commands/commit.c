#include "vls_command.h"

// add src/main.c
// stage/1234/main.c
// stage/stage < src/main.c main.c hash
// commit ...
// object/1234/main.c <- stage/1234/main.c
// commits/commits <- src/main.c / main.c / hash
int vls_commit_func(const int argc, const char **argv) {
  const char *stage_info = "stage/stage";
  return 0;
}
