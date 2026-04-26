#include "vls_command.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define COMMAND_LIST(X)                                                        \
  X(help)                                                                      \
  X(init)                                                                      \
  X(commit)                                                                    \
  X(add)                                                                       \
  X(reset)

inline const char *extract_command_name(int *argc, const char ***argv) {
  *argc -= 1;

  const char *result;
  if (sizeof(*argv) != 0) {
    result = (*argv)[0];
    (*argv)++;
  } else {
    result = "";
  }
  return result;
}

int main(int argc, const char **argv) {
  const char *command_name = extract_command_name(&argc, &argv);
  if (argc == 0) {
    vls_help.call(argc, argv);
    return EXIT_SUCCESS;
  }

  command_t command = vls_help;

#define CHECK_COMMAND(name)                                                    \
  if (strcmp(command_name, #name) == 0) {                                      \
    command = vls_##name;                                                      \
  } else

  COMMAND_LIST(CHECK_COMMAND) { command = vls_help; }

  command.call(argc, argv);
  return EXIT_SUCCESS;
}
