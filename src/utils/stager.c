#include "utils/stager.h"
#include "utils/hasher.h"
#include "utils/input_output.h"
#include "utils/logger.h"
#include "vls_paths.h"
#include "vls_types.h"
#include <cjson/cJSON.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int add_stage(const char *path) {
  int fd = 0;
  if ((fd = open(VLS_STAGE, O_RDONLY)) < 0) {
    return vls_report_errno(errno);
  };

  struct stat file;
  if (fstat(fd, &file) < 0) {
    close(fd);
    return vls_report_errno(errno);
  }

  if (file.st_size == 0) {
    goto skip_read;
  }

  char *msg_handler = "Unexpected error";
  char *map = mmap(NULL, file.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (map == MAP_FAILED) {
    msg_handler = "failed to open the stage";
    goto error_close;
  }

  cJSON *json = cJSON_ParseWithLength(map, file.st_size);
  if (!(json && cJSON_IsArray(json))) {
    cJSON_Delete(json);
    goto skip_read;
  }

  vls_md_hash_t hashNew;
  vls_md_hash_t oldHash;
  if (hash_my_path(path, &hashNew) < 0) {
    return -1;
  }

  cJSON *elem = NULL;
  cJSON_ArrayForEach(elem, json) {
    cJSON *pathCheck = cJSON_GetObjectItemCaseSensitive(elem, "path");
    cJSON *hashOld = cJSON_GetObjectItemCaseSensitive(elem, "hash");
    cJSON *status = cJSON_GetObjectItemCaseSensitive(elem, "status");

    if (!cJSON_IsString(pathCheck)) {
      msg_handler = "Path must be a string";
      goto error_close;
    }
    if (cJSON_IsString(hashOld) && (hashOld->valuestring) != NULL) {
      if (hash_from_string(hashOld->valuestring, &oldHash) < 0) {
        return -1;
      }
    } else {
      msg_handler = "Hex must be a string";
      goto error_close;
    }
    if (strcmp(path, pathCheck->valuestring) == 0) {
      munmap(map, file.st_size);
      if (memcmp(hashNew.bytes, oldHash.bytes, MD_SIZE) != 0) {
        char newHashStr[33];
        hash_to_string(&hashNew, newHashStr);
        cJSON_SetValuestring(hashOld, newHashStr);
        cJSON_SetNumberValue(status, MODIFIED);
        goto write_stage;
      } else {
        goto end_read;
      }
    }
  }
  munmap(map, file.st_size);
  goto write_stage;
skip_read:
  json = cJSON_CreateArray();
  cJSON *first_entry = cJSON_CreateObject();
  cJSON_AddStringToObject(first_entry, "path", path);

  vls_md_hash_t hash;
  if (hash_my_path(path, &hash) < 0) {
    close(fd);
    cJSON_Delete(json);
    return -1;
  }
  char hash_str[33];
  hash_to_string(&hash, hash_str);
  cJSON_AddStringToObject(first_entry, "hash", hash_str);
  cJSON_AddNumberToObject(first_entry, "status", CREATED);
  cJSON_AddItemToArray(json, first_entry);
  goto end_read;
write_stage:
  char *json_string = cJSON_PrintUnformatted(json);
  if (!json_string) {
    msg_handler = "Cannot update stage.json";
    goto error_close;
  }

  close(fd);
  if ((fd = open(VLS_STAGE, O_WRONLY | O_CREAT | O_TRUNC)) < 0) {
    msg_handler = strerror(errno);
    goto error_close;
  }

  if (!vls_safety_write((vls_output_t){fd, json_string, strlen(json_string)})) {
    free(json_string);
    return -1;
  }
  free(json_string);
end_read:
  cJSON_Delete(json);
  return 0;
error_close:
  free(json_string);
  close(fd);
  cJSON_Delete(json);
  return vls_report(msg_handler);
}

int remove_stage(const char *path);
