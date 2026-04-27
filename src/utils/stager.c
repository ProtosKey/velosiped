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

static char *path_name = "path";
static char *hash_name = "hash";
static char *status_name = "status";

int add_stage(const char *path) {
  int fd = 0;
  int out = -1;
  if ((fd = open(VLS_STAGE, O_RDONLY)) < 0) {
    return vls_report_errno(errno);
  };

  struct stat file;
  if (fstat(fd, &file) < 0) {
    close(fd);
    return vls_report_errno(errno);
  }

  vls_md_hash_t hash_new;
  if ((out = hash_my_path(path, &hash_new)) < 0) {
    close(fd);
    return out;
  }

  cJSON *json = NULL;
  bool need_new = true;
  bool need_json = true;
  bool need_write = true;
  char *msg_handler = "Unexpected error";
  if (file.st_size != 0) {
    need_json = false;
    char *map = mmap(NULL, file.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED) {
      cJSON_Delete(json);
      close(fd);
      return vls_report_errno(errno);
    }

    json = cJSON_ParseWithLength(map, file.st_size);
    if (json && cJSON_IsArray(json)) {
      vls_md_hash_t old_hash;
      cJSON *elem = NULL;

      cJSON_ArrayForEach(elem, json) {
        cJSON *path_check = cJSON_GetObjectItemCaseSensitive(elem, path_name);
        cJSON *hash_old = cJSON_GetObjectItemCaseSensitive(elem, hash_name);
        cJSON *status_old = cJSON_GetObjectItemCaseSensitive(elem, status_name);

        if (!cJSON_IsString(path_check)) {
          need_json = true;
          break;
        }

        if (cJSON_IsString(hash_old) && (hash_old->valuestring) != NULL) {
          if ((out = hash_from_string(hash_old->valuestring, &old_hash)) < 0) {
            need_json = true;
            break;
          }
        } else {
          need_json = true;
          break;
        }

        if (strcmp(path, path_check->valuestring) == 0) {
          need_new = false;
          if (memcmp(hash_new.bytes, old_hash.bytes, MD_SIZE) != 0) {
            char hash_str[33];
            hash_to_string(&hash_new, hash_str);
            cJSON_SetValuestring(hash_old, hash_str);
            cJSON_SetNumberValue(status_old, MODIFIED);
          } else {
            need_write = false;
          }
          break;
        }
      }
    } else {
      need_json = true;
    }
    munmap(map, file.st_size);
  }
  close(fd);
  if (need_write) {
    if (need_json) {
      cJSON_Delete(json);
      json = cJSON_CreateArray();
    }
    if (need_new) {
      cJSON *stage = cJSON_CreateObject();
      cJSON_AddStringToObject(stage, path_name, path);

      char hash_str[33];
      hash_to_string(&hash_new, hash_str);

      cJSON_AddStringToObject(stage, hash_name, hash_str);
      cJSON_AddNumberToObject(stage, status_name, CREATED);
      cJSON_AddItemToArray(json, stage);
    }
    char *json_string = cJSON_PrintUnformatted(json);
    if (!json_string) {
      cJSON_Delete(json);
      return vls_report("Cannot update stage.json");
    }

    cJSON_Delete(json);
    if ((fd = open(VLS_STAGE, O_WRONLY | O_CREAT | O_TRUNC)) < 0) {
      free(json_string);
      return vls_report_errno(errno);
    }

    if ((out = vls_safety_write(
             (vls_output_t){fd, json_string, strlen(json_string)})) < 0) {
      free(json_string);
      return out;
    }
    free(json_string);
  }
  return 0;
}

int remove_stage(const char *path);
