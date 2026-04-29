#include "utils/fs.h"
#include "utils/hasher.h"
#include "utils/iterator.h"
#include "utils/logger.h"
#include "utils/stager.h"
#include "utils/visitor.h"
#include "vls_command.h"
#include "vls_paths.h"
#include "vls_types.h"
#include <dirent.h>
#include <limits.h>
#include <openssl/asn1.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

typedef struct {
  node_t *tracked;
  node_t *untracked;
} track_t;

int check_file(const char *path, void *ctx) {
  track_t *context = (track_t *)ctx;

  int out;
  char root[PATH_MAX];
  if ((out = vls_find_root(root, PATH_MAX)) < 0)
    return out;
  char rel_path[PATH_MAX];
  if ((out = vls_path_from_root(rel_path, PATH_MAX, root, path)) < 0)
    return out;

  if (!check_is_in(context->tracked, rel_path)) {
    context->untracked = add_next(context->untracked, strdup(rel_path));
  }
  return 0;
}

int collect_status(stage_ctx_t *contex, void *result) {
  int out;
  char root[PATH_MAX];
  if ((out = vls_find_root(root, PATH_MAX)) < 0)
    return out;
  char abs_path[PATH_MAX];
  if ((out = vls_check_path_from_you(abs_path, PATH_MAX, contex->path)) < 0) {
    return out;
  } else if (out == 1) {
    ((status_t *)result)->deleted =
        add_next(((status_t *)result)->deleted, (void *)strdup(contex->path));
  } else {
    vls_md_hash_t hash_new;
    if ((out = hash_my_path(abs_path, &hash_new)) < 0)
      return out;

    int status = contex->status_item->valueint;
    if (status & NEW) {
      ((status_t *)result)->staged_new = add_next(
          ((status_t *)result)->staged_new, (void *)strdup(contex->path));
    } else if (status & MODIFIED) {
      ((status_t *)result)->staged_modified = add_next(
          ((status_t *)result)->staged_modified, (void *)strdup(contex->path));
    }
    vls_md_hash_t hash_old = {};
    if ((out = hash_from_string(contex->hash_item->valuestring, &hash_old)) < 0)
      return out;

    if (memcmp(hash_old.bytes, hash_new.bytes, MD_SIZE) != 0) {
      ((status_t *)result)->modified = add_next(((status_t *)result)->modified,
                                                (void *)strdup(contex->path));
    }
  }

  return 0;
}

typedef struct {
  const char *color;
  const char *type;
} output_status_t;

int output(void *data, void *pre) {
  vls_raw(CLR_BOLD);
  vls_raw(((output_status_t *)pre)->color);
  vls_raw("\t");
  vls_raw(((output_status_t *)pre)->type);
  vls_raw("\t");
  vls_say((char *)data);
  vls_raw(CLR_RESET);
  return 0;
}

int vls_status_func(const int, const char **) {
  int out;
  char root[PATH_MAX];
  if ((out = vls_find_root(root, PATH_MAX)) < 0) {
    return out;
  }

  status_t status = {};
  if ((out = check_stages(collect_status, (void *)&status)) < 0)
    return out;

  bool is_clear = true;
  bool is_new = false;

  if (!is_clear_list(status.staged_new) ||
      !is_clear_list(status.staged_modified)) {
    is_clear = false;
    vls_say("Changes to be committed:");
    if (!is_clear_list(status.staged_new)) {
      iterate(output, status.staged_new,
              &(output_status_t){CLR_GREEN, "new file:"});
      list_free(status.staged_new, true);
      is_new = true;
    }

    if (!is_clear_list(status.staged_modified)) {
      iterate(output, status.staged_modified,
              &(output_status_t){CLR_GREEN, "modified:"});
      list_free(status.staged_modified, true);
      is_new = true;
    }
  }
  if (!is_clear_list(status.modified)) {
    is_clear = false;
    if (is_new) {
      vls_raw("\n");
      is_new = false;
    }
    vls_say("Changes to be added:");
    iterate(output, status.modified, &(output_status_t){CLR_CYAN, "modified:"});
    list_free(status.modified, true);
    is_new = true;
  }
  if (!is_clear_list(status.deleted)) {
    is_clear = false;
    if (is_new) {
      vls_raw("\n");
      is_new = false;
    }
    vls_say("Changes to be dropped:");
    iterate(output, status.deleted, &(output_status_t){CLR_RED, "deleted:"});
    list_free(status.deleted, true);
    is_new = true;
  }

  char file[PATH_MAX];
  if ((out = vls_join_path(file, PATH_MAX, root, VLS_STAGE)) < 0)
    return out;

  node_t *tracked = NULL;
  if ((out = get_all_names(file, &tracked)) < 0)
    return out;

  struct {
    node_t *tracked;
    node_t *untracked;
  } walk_ctx = {.tracked = tracked, .untracked = NULL};

  if ((out = walk_dir(check_file, root, (void *)&walk_ctx)) < 0) {
    list_free(tracked, true);
    return out;
  }
  list_free(tracked, true);

  if (!is_clear_list(walk_ctx.untracked)) {
    is_clear = false;
    if (is_new) {
      vls_raw("\n");
      is_new = false;
    }
    vls_say("Files not in stage:");
    iterate(output, walk_ctx.untracked,
            &(output_status_t){CLR_RED, "untracked:"});
    list_free(walk_ctx.untracked, true);
    is_new = true;
  }

  if (is_clear) {
    vls_say("You working branch is free");
  }
  return 0;
}
