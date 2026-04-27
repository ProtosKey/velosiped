#include "vls_command.h"

command_t vls_init = {
    "init", "Initialize the lair (.md) and start the clock of history",
    vls_init_func};

command_t vls_add = {"add", "Stage files or directories for the next snapshot",
                     vls_add_func};

command_t vls_commit = {
    "commit", "Seal the current stage into an immortal history record",
    vls_commit_func};

command_t vls_reset = {"reset",
                       "Turn back time and roll back to the last clean state",
                       vls_reset_func};

command_t vls_help = {
    "help", "Show this manual for survivors and code-warriors", vls_help_func};
