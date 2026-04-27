#include <vls_types.h>

#ifndef MD_SIZE
#define MD_SIZE 16
#endif

#ifndef MD_HESHER_H
#define MD_HESHER_H

#include <openssl/md5.h>

int hash_my_path(const char *, vls_md_hash_t *);
int hash_to_string(const vls_md_hash_t *, char *);
int hash_from_string(const char *, const vls_md_hash_t *);
int is_identical(const vls_md_hash_t *, const vls_md_hash_t *);

#endif
