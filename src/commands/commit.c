#include "utils/fs.h"
#include "utils/hasher.h"
#include "utils/input_output.h"
#include "utils/logger.h"
#include "utils/time.h"
#include "vls_paths.h"
#include "vls_types.h"
#include <cjson/cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define VLS_FILES(X)                                                           \
  X(hash)                                                                      \
  X(msg)                                                                       \
  X(prev)                                                                      \
  X(time)

static bool advance_status(cJSON *item) {
  cJSON *status = cJSON_GetObjectItemCaseSensitive(item, "status");
  file_status_t s = (file_status_t)cJSON_GetNumberValue(status);
  if (s & DELETED)
    return true;
  if (s & (NEW | MODIFIED))
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

static int read_head_hash(vls_md_hash_t *out) {
  int fd = open(VLS_HEAD_FILE, O_RDONLY);
  if (fd < 0)
    return -1;

  char buf[33] = {0};
  ssize_t n = read(fd, buf, 32);
  close(fd);
  if (n != 32)
    return -1;
  return hash_from_string(buf, out);
}

static int write_head_hash(const vls_md_hash_t *hash) {
  char hash_str[33];
  hash_to_string(hash, hash_str);

  int fd = open(VLS_HEAD_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd < 0)
    return vls_report_errno_at(VLS_HEAD_FILE, errno);
  int rc = vls_safety_write((vls_output_t){fd, hash_str, strlen(hash_str)});
  close(fd);
  return rc;
}

static int save_commit(const commit_t *commit) {
  char hash_str[33];
  hash_to_string(&commit->hash, hash_str);

  char commit_path[PATH_MAX];
  if (vls_join_path(commit_path, PATH_MAX, VLS_COMMITS_DIR, hash_str) < 0)
    return -1;
  if (vls_ensure_dir(commit_path) < 0)
    return -1;

  char commit_json[PATH_MAX];
  if (vls_join_path(commit_json, PATH_MAX, commit_path, "commit.json") < 0)
    return -1;
  if (vls_copy_file(VLS_STAGE, commit_json, O_CREAT | O_WRONLY | O_TRUNC) < 0)
    return -1;

  int cmt_fd = open(commit_path, O_RDONLY | O_DIRECTORY);
  if (cmt_fd < 0)
    return vls_report_errno_at(commit_path, errno);

  int oflag = O_CREAT | O_WRONLY | O_TRUNC;
  int mod = 0644;

#define INIT(name) int name##_fd = openat(cmt_fd, #name, oflag, mod);
  VLS_FILES(INIT)
#undef INIT

  int rc = 0;
#define CHECK(name)                                                            \
  if (name##_fd < 0) {                                                         \
    rc = -1;                                                                   \
    vls_report_errno_at(#name, errno);                                         \
    goto cleanup;                                                              \
  }
  VLS_FILES(CHECK)
#undef CHECK

  vls_safety_write((vls_output_t){hash_fd, hash_str, strlen(hash_str)});

  const char *msg = commit->msg ? commit->msg : "";
  vls_safety_write((vls_output_t){msg_fd, msg, strlen(msg)});

  if (commit->prev) {
    char prev_str[33];
    hash_to_string(&commit->prev->hash, prev_str);
    vls_safety_write((vls_output_t){prev_fd, prev_str, strlen(prev_str)});
  }

  write_time_to_vls(time_fd);

cleanup:
#define CLOSE(name) close(name##_fd);
  VLS_FILES(CLOSE)
#undef CLOSE

  close(cmt_fd);
  return rc;
}

static void pack_le64(unsigned char buf[8], uint64_t v) {
  for (int i = 0; i < 8; i++)
    buf[i] = (unsigned char)((v >> (i * 8)) & 0xffu);
}

static int mark_missing_as_deleted(void) {
  int fd = open(VLS_STAGE, O_RDONLY);
  if (fd < 0)
    return vls_report_errno_at(VLS_STAGE, errno);

  cJSON *json = load_stage_json(fd);
  close(fd);
  if (!json)
    return -1;

  bool changed = false;
  cJSON *item = NULL;
  cJSON_ArrayForEach(item, json) {
    cJSON *path = cJSON_GetObjectItemCaseSensitive(item, "path");
    cJSON *status = cJSON_GetObjectItemCaseSensitive(item, "status");
    if (!cJSON_IsString(path) || !cJSON_IsNumber(status))
      continue;

    struct stat st;
    if (lstat(path->valuestring, &st) < 0 && errno == ENOENT) {
      file_status_t s = (file_status_t)cJSON_GetNumberValue(status);
      if (!(s & DELETED)) {
        cJSON_SetNumberValue(status, DELETED);
        changed = true;
      }
    }
  }

  int rc = changed ? save_stage_json(json) : 0;
  cJSON_Delete(json);
  return rc;
}

int vls_commit_func(const int argc, const char **argv) {
  if (vls_ensure_dir(VLS_COMMITS_DIR) < 0)
    return -1;

  if (mark_missing_as_deleted() < 0)
    return -1;

  commit_t parent = {0};
  bool has_parent = (read_head_hash(&parent.hash) == 0);

  struct timespec ts;
  if (clock_gettime(CLOCK_REALTIME, &ts) < 0)
    return vls_report_errno(errno);

  unsigned char salt[16];
  pack_le64(salt, (uint64_t)ts.tv_sec);
  pack_le64(salt + 8, (uint64_t)ts.tv_nsec);

  vls_md_hash_t commit_hash;
  if (hash_my_path_and_bytes(VLS_STAGE, salt, sizeof salt, &commit_hash) < 0)
    return -1;

  commit_t self = {
      .prev = has_parent ? &parent : NULL,
      .created_time = ts.tv_sec,
      .hash = commit_hash,
      .msg = (argc > 0 && argv[0]) ? argv[0] : "",
  };

  if (save_commit(&self) < 0)
    return -1;
  if (write_head_hash(&commit_hash) < 0)
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
