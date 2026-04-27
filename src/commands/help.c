#include "utils/logger.h"
#include "vls_command.h"

#define CLR_RESET "\033[0m"
#define CLR_BOLD "\033[1m"
#define CLR_CYAN "\033[36m"
#define CLR_GREEN "\033[32m"
#define CLR_WHITE "\033[37m"
#define CLR_GRAY "\033[90m"

#define COMMANDS(X)                                                            \
  X(init)                                                                      \
  X(help)                                                                      \
  X(add)                                                                       \
  X(commit)

#define PRINT_HELP(name)                                                       \
  vls_quick("\n");                                                             \
  vls_quick(vls_##name.command_name);                                          \
  vls_quick("\t—\t");                                                          \
  vls_quick(vls_##name.description);                                           \
  vls_quick("\n");

int vls_help_func(const int argc, const char **argv) {
  vls_quick("\n");
  vls_say("VeLosiped System (vls) - version control system.");

  vls_say(CLR_BOLD CLR_GREEN);
  vls_say("      (`-.                .-')    ");
  vls_say("    _(OO  )_             ( OO ).  ");
  vls_say(",--(_/   ,. \\ ,--.      (_)---\\_) ");
  vls_say("\\   \\   /(__/ |  |.-')  /    _ |  ");
  vls_say(" \\   \\ /   /  |  | OO ) \\  :` `.  ");
  vls_say("  \\   '   /,  |  |`-' |  '..`''.) ");
  vls_say("   \\     /__)(|  '---.' .-._)   \\ ");
  vls_say("    \\   /     |      |  \\       / ");
  vls_say("     `-'      `------'   `-----'  ");
  vls_say(CLR_RESET);

  COMMANDS(PRINT_HELP);

  return 0;
}
