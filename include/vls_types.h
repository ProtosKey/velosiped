#ifndef VLS_TYPES_H
#define VLS_TYPES_H

#include <time.h>

typedef struct {
  unsigned char bytes[16];
} vls_md_hash_t;

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

typedef enum { CREATED, MODIFIED, DELETED, UNCHANGED } file_status;

typedef struct commit_node {
  struct commit_node *parent;
  struct commit_node *child;
  time_t created_time;
  const char *msg;
} commit_t;

#endif
