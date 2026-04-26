#include "utils/input_output.h"
#include "vls_command.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int vls_init_func(const int, const char **) {
  const char *dirs[] = {START_DIR, COMMITS_DIR, OBJECTS_DIR, STAGE_DIR};
  for (int i = 0; i < sizeof(dirs) / sizeof(char *); i++) {
    if (mkdir(dirs[i], 0755) < 0) {
      if (errno != EEXIST) {
        goto exit_with_error;
      }
    }
  }

  int fd = 0;
  const char *name = ".vls/head";
  if ((fd = open(name, O_WRONLY | O_CREAT | O_EXCL,
                 S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
    if (errno != EEXIST) {
      goto exit_with_error;
    }
  }
  close(fd);

  const char *good = "Repository was Successfully created\n";
  vls_safety_write((vls_output_t){STDOUT_FILENO, good, strlen(good)});
  return 0;
exit_with_error:;
  const int err = errno;
  const char *msg = strerror(errno);
  vls_safety_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
  return err;
}
