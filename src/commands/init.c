#include "utils/error.h"
#include "utils/input_output.h"
#include "vls_command.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int vls_init_func(const int, const char **) {
  const char *dirs[] = {START_DIR, COMMITS_DIR, OBJECTS_DIR, STAGE_DIR};
  for (size_t i = 0; i < sizeof(dirs) / sizeof(*dirs); i++) {
    if (mkdir(dirs[i], 0755) < 0 && errno != EEXIST) {
      return vls_report_errno_at(dirs[i], errno);
    }
  }

  const char *name = ".vls/head";
  int fd = open(name, O_WRONLY | O_CREAT | O_EXCL,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0 && errno != EEXIST) {
    return vls_report_errno_at(name, errno);
  }
  if (fd >= 0)
    close(fd);

  const char *good = "Repository was Successfully created\n";
  vls_safety_write((vls_output_t){STDOUT_FILENO, good, strlen(good)});
  return 0;
}
