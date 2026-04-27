#ifndef VLS_TYPES_H
#define VLS_TYPES_H

#include <cjson/cJSON.h>
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

typedef enum { CREATED, MODIFIED, DELETED, UNCHANGED } file_status_t;

typedef struct commit_node {
  struct commit_node *parent;
  struct commit_node *child;
  time_t created_time;
  const char *msg;
} commit_t;

typedef struct {
  const file_status_t *status;
  const char *path;
  const vls_md_hash_t hash;
} stage_t;

typedef struct {
  const char *path;
  int index;
  bool need_write;
  vls_md_hash_t *hash_new;
  cJSON *hash_item;
  cJSON *status_item;
} stage_ctx_t;

typedef int (*vls_stage_action_t)(cJSON *, stage_ctx_t *ctx);

typedef struct {
  vls_stage_action_t on_found;
  vls_stage_action_t on_not_found;
} stage_ops_t;

typedef struct node_t {
  void *data;
  struct node_t *prev;
  struct node_t *next;
} node_t;

typedef struct {
  cJSON *json;
  const node_t *untracted;
  const node_t *new;
  const node_t *deleted;
  const node_t *modified;
} status_t;

#endif
