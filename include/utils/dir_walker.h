

typedef int (*vls_callback)(const char *path, void *ctx);
int walk_dir(vls_callback, const char *path, void *ctx);
