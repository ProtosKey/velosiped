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

int execute_action(const char *, const stage_ops_t *, stage_ctx_t *);

int check_stages() { return 0; }

int update_add(cJSON *json, stage_ctx_t *contex) {
  int out;
  vls_md_hash_t hash_old;
  contex->need_write = false;
  const char *msg = "Cannot check hash";
  if ((out = hash_from_string(contex->hash_item->valuestring, &hash_old)) < 0) {
    return vls_report_errno_at(msg, out);
  } else {
    if ((out = memcmp(contex->hash_new->bytes, hash_old.bytes, MD_SIZE)) < 0) {
      return vls_report_errno_at(msg, out);
    } else {
      if (out != 0) {
        char hash_str[33];
        contex->need_write = true;
        hash_to_string(contex->hash_new, hash_str);
        cJSON_SetValuestring(contex->hash_item, hash_str);
        cJSON_SetNumberValue(contex->status_item, MODIFIED);
      }
    }
  }
  return 0;
}

int update_new_add(cJSON *json, stage_ctx_t *contex) {
  cJSON *stage = cJSON_CreateObject();
  cJSON_AddStringToObject(stage, path_name, contex->path);

  char hash_str[33];
  hash_to_string(contex->hash_new, hash_str);
  cJSON_AddStringToObject(stage, hash_name, hash_str);
  cJSON_AddNumberToObject(stage, status_name, CREATED);
  cJSON_AddItemToArray(json, stage);
  return 0;
}

int add_stage(const char *path) {
  int out;
  if ((out = execute_action(path, &(stage_ops_t){*update_add, *update_new_add},
                            &(stage_ctx_t){path})) < 0) {
    return out;
  }
  return 0;
}

int remove(cJSON *json, stage_ctx_t *contex) {
  contex->need_write = true;
  cJSON_DeleteItemFromArray(json, contex->index);
  return 0;
}

int no_remove(cJSON *json, stage_ctx_t *contex) {
  contex->need_write = false;
  return 0;
}

int remove_stage(const char *path) {
  int out;
  if ((out = execute_action(path, &(stage_ops_t){*remove, *no_remove},
                            &(stage_ctx_t){path})) < 0) {
    return out;
  }
  return 0;
}

int execute_action(const char *path, const stage_ops_t *action,
                   stage_ctx_t *ctx) {
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
      cJSON *elem = NULL;

      int i = 0;
      cJSON_ArrayForEach(elem, json) {
        cJSON *path_check = cJSON_GetObjectItemCaseSensitive(elem, path_name);
        cJSON *hash_item = cJSON_GetObjectItemCaseSensitive(elem, hash_name);
        cJSON *status_item =
            cJSON_GetObjectItemCaseSensitive(elem, status_name);

        if (!cJSON_IsString(path_check)) {
          need_json = true;
          break;
        }

        if (!(cJSON_IsString(hash_item) && (hash_item->valuestring) != NULL)) {
          need_json = true;
          break;
        }

        if (strcmp(path, path_check->valuestring) == 0) {
          need_new = false;
          ctx->index = i;
          ctx->need_write = need_write;
          ctx->hash_item = hash_item;
          ctx->status_item = status_item;
          ctx->hash_new = &hash_new;
          if ((out = action->on_found(json, ctx)) < 0) {
            close(fd);
            cJSON_Delete(json);
            return out;
          }
          need_write = ctx->need_write;
        }
        i++;
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
      if ((out = action->on_not_found(json, ctx)) < 0) {
        close(fd);
        cJSON_Delete(json);
        return out;
      }
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
