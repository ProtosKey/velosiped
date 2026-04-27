#ifndef STAGER_H
#define STAGER_H

static char *path_name = "path";
static char *hash_name = "hash";
static char *status_name = "status";

int check_stages();
int add_stage(const char *);
int remove_stage(const char *);

#endif
