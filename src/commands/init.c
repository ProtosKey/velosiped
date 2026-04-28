#include "utils/fs.h"
#include "utils/logger.h"
#include "vls_command.h"
#include "vls_paths.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int vls_init_func(const int, const char **) {
  const char *dirs[] = {VLS_DIR, VLS_COMMITS_DIR, VLS_OBJECTS_DIR};
  for (size_t i = 0; i < sizeof(dirs) / sizeof(*dirs); i++) {
    int code = vls_ensure_dir(dirs[i]);
    if (code != 0)
      return code;
  }

  const char *files[] = {VLS_HEAD_FILE, VLS_STAGE};
  for (size_t i = 0; i < sizeof(files) / sizeof(*files); i++) {
    int fd = open(files[i], O_WRONLY | O_CREAT | O_EXCL,
                  S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0 && errno != EEXIST) {
      return vls_report_errno_at(files[i], errno);
    }
    close(fd);
  }

  char path[PATH_MAX];
  vls_raw("Repository was Successfully initialized");
  if (getcwd(path, PATH_MAX) &&
      vls_join_path(path, PATH_MAX, path, VLS_DIR) == 0) {
    vls_raw(" in ");
    vls_say_green(path);
  } else {
    vls_raw("\n");
  }

  return 0;
}
