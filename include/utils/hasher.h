#ifndef MD_HESHER_H
#define MD_HESHER_H
#define MD_SIZE 16

#include "vls_types.h"
#include <openssl/md5.h>
#include <stddef.h>

int hash_my_path(const char *, vls_md_hash_t *);
int hash_my_path_and_bytes(const char *, const void *, size_t, vls_md_hash_t *);
int hash_to_string(const vls_md_hash_t *, char *);
int hash_from_string(char *, vls_md_hash_t *);
int is_identical(const vls_md_hash_t *, const vls_md_hash_t *);

#endif
