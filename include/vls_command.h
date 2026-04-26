#ifndef VLS_COMMAND_H
#define VLS_COMMAND_H

typedef struct {
  const char *command_name;
  const char *description;
} command;

int vls_init();
int vls_commit();
int vls_reset();

#endif
