#include "utils/fs.h"
#include "utils/hasher.h"
#include "utils/input_output.h"
#include "utils/logger.h"
#include "vls_paths.h"
#include "vls_types.h"
#include <cjson/cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

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

static void update_stage_statuses(cJSON *json) {
  cJSON *item = json->child;
  while (item) {
    cJSON *next = item->next;
    if (advance_status(item)) {
      cJSON_DetachItemViaPointer(json, item);
      cJSON_Delete(item);
    }
    item = next;
  }
}

static int snapshot_stage_to_commit_dir(void) {
  vls_md_hash_t commit_hash;
  if (hash_my_path(VLS_STAGE, &commit_hash) < 0)
    return -1;

  char hash_str[33];
  hash_to_string(&commit_hash, hash_str);

  char commit_dir[PATH_MAX];
  if (vls_join_path(commit_dir, sizeof commit_dir, VLS_COMMITS_DIR, hash_str) <
      0)
    return -1;
  if (vls_ensure_dir(commit_dir) < 0)
    return -1;

  char commit_json[PATH_MAX];
  if (vls_join_path(commit_json, sizeof commit_json, commit_dir,
                    "commit.json") < 0)
    return -1;

  return vls_copy_file(VLS_STAGE, commit_json, O_CREAT | O_WRONLY | O_TRUNC);
}

int vls_commit_func(const int argc, const char **argv) {
  if (vls_ensure_dir(VLS_COMMITS_DIR) < 0)
    return -1;

  if (snapshot_stage_to_commit_dir() < 0)
    return -1;

  int fd = vls_ensure_file(VLS_STAGE, O_RDONLY);
  if (fd < 0)
    return -1;
  cJSON *json = load_stage_json(fd);
  close(fd);
  if (!json)
    return -1;

  update_stage_statuses(json);
  int rc = save_stage_json(json);
  cJSON_Delete(json);

  return rc < 0 ? -1 : 0;
}
