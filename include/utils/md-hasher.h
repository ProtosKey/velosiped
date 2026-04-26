#ifndef BUFFER_SIZE
#define BUFFER_SIZE 4096
#endif

#ifndef MD_SIZE
#define MD_SIZE 16
#endif

#ifndef MD_HESHER_H
#define MD_HESHER_H

#include <openssl/md5.h>

typedef struct {
  unsigned char bytes[16];
} vls_md_hash;

int hash_my_path(const char *, vls_md_hash *);
int vls_hash_to_string(const vls_md_hash *, char *);
int is_identical(const vls_md_hash *, const vls_md_hash *);

#endif
