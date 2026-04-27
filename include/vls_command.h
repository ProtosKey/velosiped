
#ifndef VLS_COMMAND_H
#define VLS_COMMAND_H

#include "vls_types.h"

int vls_init_func(const int, const char **);
int vls_add_func(const int, const char **);
int vls_commit_func(const int, const char **);
int vls_reset_func(const int, const char **);
int vls_help_func(const int, const char **);
int vls_status_funct(const int, const char **);

extern command_t vls_init;
extern command_t vls_add;
extern command_t vls_commit;
extern command_t vls_reset;
extern command_t vls_help;
extern command_t vls_status;

#endif
