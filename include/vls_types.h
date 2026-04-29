#ifndef VLS_TYPES_H
#define VLS_TYPES_H

#include <cjson/cJSON.h>
#include <time.h>

typedef struct {
  unsigned char bytes[16];
} vls_md_hash_t;

typedef struct {
  const int fd;
  const char *message;
  const size_t size;
} vls_output_t;

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

typedef enum {
  UNCHANGED = 0b0,
  NEW = 0b1,
  MODIFIED = 0b10,
  DELETED = 0b100,
  UNTRACTED = 0b1000
} file_status_t;

typedef struct commit_node {
  struct commit_node *prev;
  time_t created_time;
  vls_md_hash_t hash;
  const char *msg;
} commit_t;

typedef struct {
  file_status_t status;
  vls_md_hash_t hash;
  const char *path;
} stage_t;

typedef struct {
  const char *path;
  char *abs_path;
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
  node_t *staged_new;
  node_t *staged_modified;
  node_t *deleted;
  node_t *modified;
} status_t;

#endif
