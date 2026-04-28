#ifndef STAGER_H
#define STAGER_H

#include "vls_types.h"

static char *path_name = "path";
static char *hash_name = "hash";
static char *status_name = "status";

int check_stages();
int check_stage(const char *, stage_t *stage);
int add_stage(const char *);
int remove_stage(const char *);

#endif
