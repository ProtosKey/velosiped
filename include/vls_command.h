#ifndef VLS_COMMAND_H
#define VLS_COMMAND_H

typedef struct {
  const char *command_name;
  const char *description;
  int (*ptr)(const int c, const char **argv);
} command_t;

int vls_init_func(const int c, const char **argv);
command_t vls_init = {"init", "-", vls_init_func};
int vls_add_func(const int c, const char **argv);
command_t vls_add = {"init", "-", vls_add_func};
int vls_commit_func(const int c, const char **argv);
command_t vls_commit = {"init", "-", vls_commit_func};
int vls_reset_func(const int c, const char **argv);
command_t vls_reset = {"init", "-", vls_reset_func};
int vls_help_func(const int c, const char **argv);
command_t vls_help = {"init", "-", vls_help_func};

#endif
