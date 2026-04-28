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
  if ((out = hash_from_string(contex->hash_item->string, &hash_old)) < 0)
    return out;

  if (!memcmp(hash_old.bytes, contex->hash_new->bytes, MD_SIZE)) {
    ((status_t *)result)->modified =
        add_next(((status_t *)result)->modified, (void *)contex->path);
  }

  return 0;
};

int output(void *data, void *) {
  vls_say((char *)data);
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
  iterate(output, status.staged_new, NULL);
  iterate(output, status.staged_modified, NULL);

  if ((out = walk_dir(check_file, root, (void *)&status)) < 0) {
    return out;
  }
  return 0;
}
