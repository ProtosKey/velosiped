#include "vls_command.h"
#include <string.h>
#include <unistd.h>

#define COMMAND_LIST(X)                                                        \
  X(init)                                                                      \
  X(help)                                                                      \
  X(commit)                                                                    \
  X(add)                                                                       \
  X(reset)                                                                     \
  X(status)

static inline const char *extract_command_name(int *argc, const char ***argv) {
  *argc -= 1;
  (*argv)++;

  const char *result;
  if (*argc > 0) {
    result = (*argv)[0];
    *argc -= 1;
    (*argv)++;
  } else {
    result = "";
  }
  return result;
}

int main(int argc, const char **argv) {
  const char *command_name = extract_command_name(&argc, &argv);

  command_t command = vls_help;

#define CHECK_COMMAND(name)                                                    \
  if (strcmp(command_name, #name) == 0) {                                      \
    command = vls_##name;                                                      \
  } else

  COMMAND_LIST(CHECK_COMMAND) { command = vls_help; }

  return command.call(argc, argv);
}
