#define _POSIX_C_SOURCE 200809L
#include "utils/dir_walker.h"
#include "utils/error.h"
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static bool should_skip(const char *name) {
  return strcmp(name, ".") == 0 || strcmp(name, "..") == 0 ||
         strcmp(name, ".vls") == 0;
}

static int join_path(char *out, size_t cap, const char *dir, const char *name) {
  size_t dlen = strlen(dir);
  const char *sep = (dlen > 0 && dir[dlen - 1] != '/') ? "/" : "";
  int n = snprintf(out, cap, "%s%s%s", dir, sep, name);
  if (n < 0 || (size_t)n >= cap) {
    errno = ENAMETOOLONG;
    return -1;
  }
  return 0;
}

static int walk_directory(vls_callback_t callback, const char *path,
                          void *ctx) {
  DIR *d = opendir(path);
  if (!d)
    return vls_report_errno_at(path, errno);

  int rc = 0;
  struct dirent *e;
  while ((e = readdir(d)) != NULL) {
    if (should_skip(e->d_name))
      continue;

    char child[PATH_MAX];
    if (join_path(child, sizeof child, path, e->d_name) < 0) {
      rc = vls_report_errno_at(path, errno);
      break;
    }
    rc = walk_dir(callback, child, ctx);
    if (rc < 0)
      break;
  }
  closedir(d);
  return rc;
}

int walk_dir(vls_callback_t callback, const char *path, void *ctx) {
  struct stat st;
  if (lstat(path, &st) < 0)
    return vls_report_errno_at(path, errno);

  if (S_ISREG(st.st_mode))
    return callback(path, ctx);
  if (S_ISDIR(st.st_mode))
    return walk_directory(callback, path, ctx);

  return vls_report("Not a file, not a directory");
}
