#include "vls_command.h"

command_t vls_init = {
    "init", "Initialize the repository and start the clock of you history",
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

command_t vls_status = {
    "status", "Show the current state of the staging area and tracked files",
    vls_status_func};

command_t vls_log = {
    "log", "Walk back through every commit that brought you to this moment",
    vls_log_func};

command_t vls_drop = {
    "drop", "Yank a file out of the staging area without disturbing the disk",
    vls_drop_func};
