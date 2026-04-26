#ifndef STAGE_H
#define STAGE_H

#include "md_hasher.h"
#include "vls_command.h"

typedef struct {
  const char *path;
  const vls_md_hash_t *hash;
  file_status status;
} stage;

int read_stage(stage **);
int add_stage(stage *);
int delete_stage(stage *);

#endif
