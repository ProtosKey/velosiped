#ifndef VLS_COMMAND_H
#define VLS_COMMAND_H

typedef struct {
  const char *command_name;
  const char *description;
  const char **args;
} command_data_t;

int vls_init(const command_data_t command_data);
int vls_commit(const command_data_t command_data);
int vls_reset(const command_data_t command_data);

#endif
