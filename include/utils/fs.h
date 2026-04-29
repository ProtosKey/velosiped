#include <stddef.h>

#ifndef FS_H
#define FS_H

int vls_ensure_dir(const char *);
int vls_ensure_file(const char *, int, ...);
int vls_join_path(char *, size_t, const char *, const char *);
int vls_copy_file(const char *, const char *, int);
int vls_find_root(char *, size_t);
int vls_path_from_root(char *, size_t, const char *, const char *);
int vls_check_path_from_you(char *, size_t, const char *);

#endif
