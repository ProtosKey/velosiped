#include "vls_command.h"

command_t vls_init = {"init", "-", vls_init_func};
command_t vls_add = {"add", "-", vls_add_func};
command_t vls_commit = {"commit", "-", vls_commit_func};
command_t vls_reset = {"reset", "-", vls_reset_func};
command_t vls_help = {"help", "-", vls_help_func};
