#include "utils/vls_writer.h"
#include "vls_command.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int vls_init(const command_data command_data) {
  const char *dirs[] = {
      ".vls", ".vls/temp",
      // ".vls/commits",
      // ".vls/objects",
      // ".vls/stage",
  };
  for (int i = 0; i < sizeof(dirs) / sizeof(char *); i++) {
    if (mkdir(dirs[i], 0755) < 0) {
      if (errno != EEXIST) {
        goto exit_with_error;
      }
    }
  }

  int fd = 0;
  const char *name = ".vls/head";
  if ((fd = creat(name, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
    if (errno != EEXIST) {
      goto exit_with_error;
    }
  }
  close(fd);

  const char *good = "Repository was Successfully created";
  vls_write((vls_output_t){STDOUT_FILENO, good, strlen(good)});
  return 0;
exit_with_error:;
  const char *msg = strerror(errno);
  vls_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
  return errno;
}
