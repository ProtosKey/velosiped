#include "utils/input_output.h"
#include "utils/stage_reader.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int read_stage(stage **stages) {
  char path_stage[BUFFER_SIZE];
  int len = snprintf(path_stage, BUFFER_SIZE, "%s/%s", STAGE_DIR, "stage");

  int fd = 0;
  if ((fd = open(path_stage, O_RDONLY)) < 0) {
    goto read_stages_error;
  }

  struct stat file;
  if (fstat(fd, &file) < 0) {
    goto read_stages_error;
  }

  {
    auto size = file.st_size + 1;
    char buff[size];

    while (true) {
      ssize_t bytes_read = read(fd, buff, size);
      if (bytes_read < 0) {
        goto read_stages_error;
      } else if (bytes_read == 0) {
        break;
      }
    }
  }
  return 0;
read_stages_error:
  close(fd);
  const char *msg = strerror(errno);
  vls_safety_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
  return -1;
}

int add_stage(stage *stage) { return 0; }

int delete_stage(stage *stage) { return 0; }
