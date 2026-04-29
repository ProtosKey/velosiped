#ifndef STAGER_H
#define STAGER_H

#include "vls_types.h"

static char *path_name = "path";
static char *hash_name = "hash";
static char *status_name = "status";

typedef int (*collect_data)(stage_ctx_t *, void *);

int get_all_names(const char *, node_t **);
int check_stages(collect_data, void *);
int check_stage(const char *, stage_t *stage);
int add_stage(const char *);
int remove_stage(const char *);

#endif
