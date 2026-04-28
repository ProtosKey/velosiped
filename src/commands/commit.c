#include "utils/fs.h"
#include "utils/hasher.h"
#include "utils/input_output.h"
#include "utils/logger.h"
#include "vls_paths.h"
#include "vls_types.h"
#include <cjson/cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static int parse_stage_entry(const cJSON *item, stage_t *out) {
  const cJSON *path = cJSON_GetObjectItemCaseSensitive(item, "path");
  const cJSON *status = cJSON_GetObjectItemCaseSensitive(item, "status");
  const cJSON *hash = cJSON_GetObjectItemCaseSensitive(item, "hash");

  if (!cJSON_IsString(path) || !cJSON_IsNumber(status) || !cJSON_IsString(hash))
    return -1;

  out->path = path->valuestring;
  out->status = (file_status_t)cJSON_GetNumberValue(status);
  return hash_from_string(hash->valuestring, &out->hash);
}

static int copy_blob_to_objects(const stage_t *entry, int stage_dir,
                                int obj_dir) {
  if (entry->status & (DELETED | UNTRACTED))
    return 0;

  char hash_str[33];
  hash_to_string(&entry->hash, hash_str);

  int src_fd = openat(stage_dir, hash_str, O_RDONLY);
  int dst_fd = openat(obj_dir, hash_str, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if (src_fd < 0 || dst_fd < 0) {
    if (src_fd >= 0)
      close(src_fd);
    if (dst_fd >= 0)
      close(dst_fd);
    return -1;
  }

  int rc = vls_safety_copy(src_fd, dst_fd);
  close(src_fd);
  close(dst_fd);
  return rc;
}

static bool advance_status(cJSON *item) {
  cJSON *status = cJSON_GetObjectItemCaseSensitive(item, "status");
  file_status_t s = (file_status_t)cJSON_GetNumberValue(status);
  if (s & DELETED)
    return true;
  if (s & (CREATED | MODIFIED))
    cJSON_SetNumberValue(status, UNCHANGED);
  return false;
}

static cJSON *load_stage_json(int fd) {
  struct stat st;
  if (fstat(fd, &st) < 0) {
    vls_report_errno(errno);
    return NULL;
  }

  void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (map == MAP_FAILED) {
    vls_report_errno(errno);
    return NULL;
  }

  cJSON *json = cJSON_ParseWithLength(map, st.st_size);
  munmap(map, st.st_size);

  if (!json) {
    const char *err = cJSON_GetErrorPtr();
    vls_report_at("cJSON", err ? err : "parse error");
  }
  return json;
}

static int save_stage_json(const cJSON *json) {
  char *text = cJSON_PrintUnformatted(json);
  if (!text)
    return vls_report("cJSON_Print failed");

  int fd = open(VLS_STAGE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0) {
    free(text);
    return vls_report_errno_at(VLS_STAGE, errno);
  }

  int rc = vls_safety_write((vls_output_t){fd, text, strlen(text)});
  close(fd);
  free(text);
  return rc;
}

static int apply_commit(cJSON *json, int stage_dir, int obj_dir) {
  cJSON *item = json->child;
  while (item) {
    cJSON *next = item->next;

    stage_t entry;
    if (parse_stage_entry(item, &entry) < 0)
      return vls_report("Unexpected error while parsing stage");

    if (copy_blob_to_objects(&entry, stage_dir, obj_dir) < 0)
      return vls_report_at("dump", "Failed to dump object from stage");

    if (advance_status(item)) {
      cJSON_DetachItemViaPointer(json, item);
      cJSON_Delete(item);
    }
    item = next;
  }
  return 0;
}

int vls_commit_func(const int argc, const char **argv) {
  if (vls_ensure_dir(VLS_STAGE_DIR) < 0 || vls_ensure_dir(VLS_COMMITS_DIR) < 0)
    return -1;

  if (vls_copy_file(VLS_STAGE, VLS_COMMIT, O_CREAT | O_WRONLY | O_TRUNC) < 0)
    return -1;

  int fd = vls_ensure_file(VLS_STAGE, O_RDONLY);
  if (fd < 0)
    return -1;
  cJSON *json = load_stage_json(fd);
  close(fd);
  if (!json)
    return -1;

  int stage_dir = open(VLS_STAGE_DIR, O_RDONLY | O_DIRECTORY);
  int obj_dir = open(VLS_OBJECTS_DIR, O_RDONLY | O_DIRECTORY);

  int rc = -1;
  if (stage_dir < 0 || obj_dir < 0) {
    vls_report_errno(errno);
  } else {
    rc = apply_commit(json, stage_dir, obj_dir);
    if (rc == 0)
      rc = save_stage_json(json);
  }

  if (stage_dir >= 0)
    close(stage_dir);
  if (obj_dir >= 0)
    close(obj_dir);
  cJSON_Delete(json);
  return rc < 0 ? -1 : 0;
}
