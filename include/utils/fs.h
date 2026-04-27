#include <stddef.h>

int vls_ensure_dir(const char *path);
int vls_ensure_file(const char *path, int oflag, ...);
int vls_join_path(char *out, size_t cap, const char *dir, const char *name);
int vls_copy_file(const char *src, const char *dst, int dst_oflag);
