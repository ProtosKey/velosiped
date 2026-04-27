#include "utils/fs.h"
#include "utils/input_output.h"
#include "utils/logger.h"
#include "vls_command.h"
#include "vls_paths.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int vls_init_func(const int, const char **) {
  const char *dirs[] = {VLS_DIR, VLS_COMMITS_DIR, VLS_OBJECTS_DIR,
                        VLS_STAGE_DIR};
  for (size_t i = 0; i < sizeof(dirs) / sizeof(*dirs); i++) {
    int code = vls_ensure_dir(dirs[i]);
    if (code != 0)
      return code;
  }

  const char *name = VLS_HEAD_FILE;
  int fd = open(name, O_WRONLY | O_CREAT | O_EXCL,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0 && errno != EEXIST) {
    return vls_report_errno_at(name, errno);
  }

  close(fd);
  const char *good = "Repository was Successfully created\n";
  vls_safety_write((vls_output_t){STDOUT_FILENO, good, strlen(good)});
  return 0;
}
