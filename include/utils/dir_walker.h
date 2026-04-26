#ifndef VLS_DIR_WALKER_H
#define VLS_DIR_WALKER_H

typedef int (*vls_callback_t)(const char *path, void *ctx);
int walk_dir(vls_callback_t callback, const char *path, void *ctx);

#endif
