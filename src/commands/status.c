#include "utils/fs.h"
#include "utils/hasher.h"
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

int check_file(const char *path, void *ctx) {
  /*
int out;
stage_t stage = {path};
if ((out = check_stage(path, &stage)) < 0) {
  return out;
}
if (stage.status == UNTRACTED) {
  ((status_t *)ctx)->untracted->next =
      &(node_t){(void *)&stage, ((status_t *)ctx)->untracted, NULL};
  ((status_t *)ctx)->untracted = ((status_t *)ctx)->untracted->next;
} else {
  if (stage.status == CREATED) {
    ((status_t *)ctx)->new->next =
        &(node_t){(void *)&stage, ((status_t *)ctx)->new, NULL};
    ((status_t *)ctx)->new = ((status_t *)ctx)->new->next;
  } else if (stage.status == MODIFIED) {
    ((status_t *)ctx)->modified->next =
        &(node_t){(void *)&stage, ((status_t *)ctx)->modified, NULL};
    ((status_t *)ctx)->modified = ((status_t *)ctx)->modified->next;
  } else if (stage.status == DELETED) {
    ((status_t *)ctx)->deleted->next =
        &(node_t){(void *)&stage, ((status_t *)ctx)->deleted, NULL};
    ((status_t *)ctx)->deleted = ((status_t *)ctx)->deleted->next;
  } else if (stage.status == UNCHANGED) {
    ((status_t *)ctx)->old->next =
        &(node_t){(void *)&stage, ((status_t *)ctx)->old, NULL};
    ((status_t *)ctx)->old = ((status_t *)ctx)->old->next;
  }
  vls_md_hash_t new_hash;
  if ((out = hash_my_path(path, &new_hash)) < 0) {
    return out;
  }
  if (!memcmp(stage.hash.bytes, new_hash.bytes, MD_SIZE)) {
    ctx
  }
}
 */
  return 0;
}

int vls_status_funct(const int, const char **) {
  int out;
  char root[PATH_MAX];
  if ((out = vls_find_root(root, PATH_MAX)) < 0) {
    return out;
  }

  status_t status = {};
  if ((out = walk_dir(check_file, root, (void *)&status)) < 0) {
    return out;
  }
  return 0;
}
