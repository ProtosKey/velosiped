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
  X(commit)                                                                    \
  X(add)                                                                       \
  X(reset)                                                                     \
  X(status)

#define PRINT_HELP(name)                                                       \
  vls_raw("\n\t");                                                             \
  vls_raw_green(vls_##name.command_name);                                      \
  vls_raw("\t");                                                               \
  vls_raw(vls_##name.description);

int vls_help_func(const int argc, const char **argv) {
  vls_raw_green("\tVeLosiped System (vls)");
  vls_say("\tversion control system.\n");

  vls_raw("A hardcore version control system written in pure C using POSIX "
          "system calls.\n");

  vls_say("\t      (`-.                .-')    ");
  vls_say("\t    _(OO  )_             (  OO).  ");
  // vls_say("\t,--(_/   ,. \\ ,--.      (_)---\\_) ");

  vls_raw_green("\t,--");
  vls_raw("(_/   ");
  vls_raw_green(",. ");
  vls_raw("\\ ");
  vls_raw_green(",--.      ");
  vls_raw("(_)");
  vls_raw_green("---");
  vls_say("\\_) ");

  // vls_say("\t\\   \\   /(__/ |  |.-')  /    _ |  ");

  vls_raw_green("\t\\   \\   /");
  vls_raw("(__/ ");
  vls_raw_green("|  |");
  vls_raw(".-')");
  vls_say_green("  /    _ |  ");

  // vls_say("\t \\   \\ /   /  |  | OO ) \\  :` `.  ");

  vls_raw_green("\t \\   \\ /   /  |  | ");
  vls_raw("OO ) ");
  vls_say_green("\\  :` `.  ");

  // vls_say("\t  \\   '   /,  |  |`-' |  '..`''.) ");

  vls_raw_green("\t  \\   '   /");
  vls_raw(",  ");
  vls_raw_green("|  |");
  vls_raw("`-' |  ");
  vls_raw_green("'..`''.");
  vls_say(") ");

  // vls_say("\t   \\     /__)(|  '---.' .-._)   \\ ");

  vls_raw_green("\t   \\     /");
  vls_raw("__)(");
  vls_raw_green("|  '---.");
  vls_raw("' ");
  vls_say_green(".-._)   \\ ");

  vls_say_green("\t    \\   /     |      |  \\       / ");
  vls_say_green("\t     `-'      `------'   `-----'  ");

  vls_say("The utility has the following commands for easy manipulation of "
          "your data.");

  COMMANDS(PRINT_HELP);
  vls_say(CLR_RESET);

  return 0;
}
