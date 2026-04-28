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
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int dump_file_to_object(stage_t *stage_data, int stage_dir, int obj_dir) {
  if (stage_data->status & (DELETED | UNTRACTED)) {
    return 0;
  }

  char hash_str[33];
  hash_to_string(&stage_data->hash, hash_str);

  int src_fd = openat(stage_dir, hash_str, O_RDONLY);
  int dst_fd = openat(obj_dir, hash_str, O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if (src_fd < 0 || dst_fd < 0)
    return -1;

  int rc = vls_safety_copy(src_fd, dst_fd);
  close(src_fd);
  close(dst_fd);
  return rc;
}

static int stage_from_json(const cJSON *item, stage_t *out) {
  const cJSON *path = cJSON_GetObjectItemCaseSensitive(item, "path");
  const cJSON *status = cJSON_GetObjectItemCaseSensitive(item, "status");
  const cJSON *hash = cJSON_GetObjectItemCaseSensitive(item, "hash");

  if (!cJSON_IsString(path) || !cJSON_IsNumber(status) ||
      !cJSON_IsString(hash)) {
    return -1;
  }

  out->status = cJSON_GetNumberValue(status);
  out->path = path->valuestring;

  int rc = hash_from_string(hash->valuestring, &out->hash);

  return rc < 0 ? -1 : 0;
}

int handle_stage_by_fd(int fd) {
  const char *data = MAP_FAILED;
  struct stat file_stat = {0};
  cJSON *json = NULL;
  int stage_dir = open(VLS_STAGE_DIR, O_RDONLY | O_DIRECTORY);
  int obj_dir = open(VLS_OBJECTS_DIR, O_RDONLY | O_DIRECTORY);
  int result = 0;

  if (fstat(fd, &file_stat) < 0 || stage_dir < 0 || obj_dir < 0) {
    vls_report_errno(errno);
    result = -1;
    goto exit;
  };

  data = mmap(NULL, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);

  if (data == MAP_FAILED) {
    vls_report_errno(errno);
    result = -1;
    goto exit;
  }

  json = cJSON_ParseWithLength(data, file_stat.st_size);
  if (!json) {
    const char *error_ptr = cJSON_GetErrorPtr();
    if (error_ptr) {
      vls_report_at("cJSON", error_ptr);
    }
    result = -1;
    goto exit;
  }

  ssize_t arr_size = cJSON_GetArraySize(json);
  for (int i = 0; i < arr_size; i++) {
    cJSON *item = cJSON_GetArrayItem(json, i);

    stage_t out;
    int rc = stage_from_json(item, &out);
    if (rc < 0) {
      vls_report("Unexpected error while parsing stage");
      result = -1;
      goto exit;
    }
    if (dump_file_to_object(&out, stage_dir, obj_dir) < 0) {
      vls_report_at("dump", "Exception while dumping object from stage");
      result = -1;
      goto exit;
    }
  }

exit:
  close(stage_dir);
  close(obj_dir);
  if (json)
    cJSON_Delete(json);
  if (data != MAP_FAILED)
    munmap((void *)data, file_stat.st_size);
  return result;
}

int vls_commit_func(const int argc, const char **argv) {
  if (vls_ensure_dir(VLS_STAGE_DIR) < 0)
    return -1;
  int fd = vls_ensure_file(VLS_STAGE, O_RDONLY);
  if (fd < 0)
    return -1;
  int rc = handle_stage_by_fd(fd);
  close(fd);
  return rc < 0 ? -1 : 0;
}
