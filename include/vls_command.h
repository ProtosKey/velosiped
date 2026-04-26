#ifndef VLS_COMMAND_H
#define VLS_COMMAND_H

#include "utils/md-hasher.h"

typedef struct {
  const char *command_name;
  const char *description;
  int (*call)(const int, const char **);
} command_t;

typedef struct {
  const vls_md_hash_t *hash;
  const char *path;
  const char *name;
} object_t;

int vls_init_func(const int, const char **);
int vls_add_func(const int, const char **);
int vls_commit_func(const int, const char **);
int vls_reset_func(const int, const char **);
int vls_help_func(const int, const char **);

extern command_t vls_init;
extern command_t vls_add;
extern command_t vls_commit;
extern command_t vls_reset;
extern command_t vls_help;

#endif
