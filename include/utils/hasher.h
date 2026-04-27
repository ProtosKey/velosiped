#include <vls_types.h>

#ifndef MD_HESHER_H
#define MD_HESHER_H
#define MD_SIZE 16

#include <openssl/md5.h>

int hash_my_path(const char *, vls_md_hash_t *);
int hash_to_string(const vls_md_hash_t *, char *);
int is_identical(const vls_md_hash_t *, const vls_md_hash_t *);

#endif
