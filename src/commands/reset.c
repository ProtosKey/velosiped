#include "utils/fs.h"
#include "utils/input_output.h"
#include "utils/logger.h"
#include "vls_command.h"
#include "vls_paths.h"
#include "vls_types.h"
#include <cjson/cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define HASH_LEN 32

static int ensure_parent_dirs(const char *path) {
  char buf[PATH_MAX];
  size_t n = strlen(path);
  if (n >= sizeof buf) {
    errno = ENAMETOOLONG;
    return -1;
  }
  memcpy(buf, path, n + 1);

  for (char *p = buf + 1; *p; p++) {
    if (*p == '/') {
      *p = '\0';
      if (mkdir(buf, 0755) < 0 && errno != EEXIST)
        return vls_report_errno_at(buf, errno);
      *p = '/';
    }
  }
  return 0;
}

static cJSON *parse_json_file(const char *path) {
  int fd = open(path, O_RDONLY);
  if (fd < 0) {
    vls_report_errno_at(path, errno);
    return NULL;
  }
  struct stat st;
  if (fstat(fd, &st) < 0) {
    vls_report_errno(errno);
    close(fd);
    return NULL;
  }
  void *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  close(fd);
  if (map == MAP_FAILED) {
    vls_report_errno(errno);
    return NULL;
  }
  cJSON *json = cJSON_ParseWithLength(map, st.st_size);
  munmap(map, st.st_size);
  return json;
}

static int restore_blob(const char *path, const char *hash_str) {
  if (ensure_parent_dirs(path) < 0)
    return -1;

  char src[PATH_MAX];
  if (vls_join_path(src, sizeof src, VLS_OBJECTS_DIR, hash_str) < 0)
    return -1;

  return vls_copy_file(src, path, O_WRONLY | O_CREAT | O_TRUNC);
}

static int restore_working_tree(cJSON *json) {
  cJSON *item;
  cJSON_ArrayForEach(item, json) {
    cJSON *path = cJSON_GetObjectItemCaseSensitive(item, "path");
    cJSON *status = cJSON_GetObjectItemCaseSensitive(item, "status");
    cJSON *hash = cJSON_GetObjectItemCaseSensitive(item, "hash");
    if (!cJSON_IsString(path) || !cJSON_IsNumber(status) ||
        !cJSON_IsString(hash))
      continue;

    file_status_t s = (file_status_t)cJSON_GetNumberValue(status);
    if (s & DELETED)
      continue;

    if (restore_blob(path->valuestring, hash->valuestring) < 0)
      return -1;
  }
  return 0;
}

static void normalize_for_post_reset(cJSON *json) {
  cJSON *item = json->child;
  while (item) {
    cJSON *next = item->next;
    cJSON *status = cJSON_GetObjectItemCaseSensitive(item, "status");
    file_status_t s = cJSON_IsNumber(status)
                          ? (file_status_t)cJSON_GetNumberValue(status)
                          : 0;
    if (s & DELETED) {
      cJSON_DetachItemViaPointer(json, item);
      cJSON_Delete(item);
    } else if (cJSON_IsNumber(status)) {
      cJSON_SetNumberValue(status, UNCHANGED);
    }
    item = next;
  }
}

static int write_text(const char *path, const char *text, size_t n) {
  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0)
    return vls_report_errno_at(path, errno);
  int rc = vls_safety_write((vls_output_t){fd, text, n});
  close(fd);
  return rc;
}

int vls_reset_func(const int argc, const char **argv) {
  if (argc < 1)
    return vls_report("Target commit hash required");

  const char *target = argv[0];
  if (strlen(target) != HASH_LEN)
    return vls_report("Hash must be 32 hex chars");

  char commit_dir[PATH_MAX];
  if (vls_join_path(commit_dir, sizeof commit_dir, VLS_COMMITS_DIR, target) < 0)
    return -1;

  struct stat st;
  if (stat(commit_dir, &st) < 0 || !S_ISDIR(st.st_mode))
    return vls_report("No such commit");

  char commit_json[PATH_MAX];
  if (vls_join_path(commit_json, sizeof commit_json, commit_dir,
                    "commit.json") < 0)
    return -1;

  cJSON *json = parse_json_file(commit_json);
  if (!json)
    return -1;

  int rc = -1;

  if (restore_working_tree(json) < 0)
    goto cleanup;

  normalize_for_post_reset(json);

  char *text = cJSON_PrintUnformatted(json);
  if (!text) {
    vls_report("cJSON_Print failed");
    goto cleanup;
  }
  rc = write_text(VLS_STAGE, text, strlen(text));
  free(text);
  if (rc < 0)
    goto cleanup;

  rc = write_text(VLS_HEAD_FILE, target, HASH_LEN);

cleanup:
  cJSON_Delete(json);
  return rc < 0 ? -1 : 0;
}
