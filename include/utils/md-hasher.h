#ifndef MD_HESHER_H
#define MD_HESHER_H

typedef struct {
  unsigned char bytes[16];
} vls_md_hash;

int hash_my_path(const char *path, vls_md_hash *result);
int vls_hash_to_string(const vls_md_hash *hash, char *result);

#endif
