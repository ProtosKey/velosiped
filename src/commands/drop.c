#include "utils/input_output.h"
#include "utils/logger.h"
#include "vls_command.h"
#include "vls_paths.h"
#include <cjson/cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

static cJSON *load_stage(void) {
  int fd = open(VLS_STAGE, O_RDONLY);
  if (fd < 0) {
    vls_report_errno_at(VLS_STAGE, errno);
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

static int save_stage(const cJSON *json) {
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

int vls_drop_func(const int argc, const char **argv) {
  if (argc < 1)
    return vls_report("drop: file path required");

  cJSON *json = load_stage();
  if (!json)
    return -1;

  bool dropped = false;
  cJSON *item = json->child;
  while (item) {
    cJSON *next = item->next;
    cJSON *path = cJSON_GetObjectItemCaseSensitive(item, "path");
    if (cJSON_IsString(path) && strcmp(path->valuestring, argv[0]) == 0) {
      cJSON_DetachItemViaPointer(json, item);
      cJSON_Delete(item);
      dropped = true;
      break;
    }
    item = next;
  }

  int rc = 0;
  if (!dropped) {
    rc = vls_report_at(argv[0], "not in stage");
  } else {
    rc = save_stage(json);
  }
  cJSON_Delete(json);
  return rc < 0 ? -1 : 0;
}
