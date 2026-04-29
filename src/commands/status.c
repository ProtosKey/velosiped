#include "utils/fs.h"
#include "utils/hasher.h"
#include "utils/iterator.h"
#include "utils/logger.h"
#include "utils/stager.h"
#include "utils/visitor.h"
#include "vls_command.h"
#include "vls_types.h"
#include <dirent.h>
#include <limits.h>
#include <openssl/asn1.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int check_file(const char *path, void *ctx) { return 0; }

int collect_status(stage_ctx_t *contex, void *result) {
  int status = contex->status_item->valueint;
  if (status & NEW) {
    ((status_t *)result)->staged_new =
        add_next(((status_t *)result)->staged_new, (void *)contex->path);
  } else if (status & MODIFIED) {
    ((status_t *)result)->staged_modified =
        add_next(((status_t *)result)->staged_modified, (void *)contex->path);
  }

  int out;
  vls_md_hash_t hash_old = {};
  if ((out = hash_from_string(contex->hash_item->valuestring, &hash_old)) < 0)
    return out;

  if (!memcmp(hash_old.bytes, contex->hash_new->bytes, MD_SIZE)) {
    ((status_t *)result)->modified =
        add_next(((status_t *)result)->modified, (void *)contex->path);
  }

  return 0;
};

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

int vls_status_funct(const int, const char **) {
  int out;
  char root[PATH_MAX];
  if ((out = vls_find_root(root, PATH_MAX)) < 0) {
    return out;
  }

  status_t status = {};
  check_stages(collect_status, (void *)&status);

  vls_say("Changes to be committed:");
  iterate(output, status.staged_new,
          &(output_status_t){CLR_GREEN, "new file:"});

  vls_raw("\n");
  iterate(output, status.staged_modified,
          &(output_status_t){CLR_CYAN, "modified:"});

  vls_raw("\n");
  iterate(output, status.staged_modified,
          &(output_status_t){CLR_CYAN, "modified:"});

  if ((out = walk_dir(check_file, root, (void *)&status)) < 0) {
    return out;
  }
  return 0;
}
