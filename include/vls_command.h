#ifndef VLS_COMMAND_H
#define VLS_COMMAND_H

typedef struct {
  const char *command_name;
  const char *description;
  int (*call)(const int, const char **);
} command_t;

int vls_init_func(const int, const char **);
int vls_add_func(const int, const char **);
int vls_commit_func(const int, const char **);
int vls_reset_func(const int, const char **);
int vls_help_func(const int, const char **);

command_t vls_init = {"init", "-", vls_init_func};
command_t vls_add = {"init", "-", vls_add_func};
command_t vls_commit = {"init", "-", vls_commit_func};
command_t vls_reset = {"init", "-", vls_reset_func};
command_t vls_help = {"init", "-", vls_help_func};

#endif
