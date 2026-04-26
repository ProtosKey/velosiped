#include "utils/input_output.h"
#include "utils/md_hasher.h"
#include <errno.h>
#include <fcntl.h>
#include <openssl/evp.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int hash_my_path(const char *path, vls_md_hash_t *result) {
  int fd = 0;
  if ((fd = open(path, O_RDONLY)) < 0) {
    const char *msg = strerror(errno);
    vls_safety_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
    return -1;
  }

  struct stat file;
  if (stat(path, &file) < 0) {
    const char *msg = strerror(errno);
    vls_safety_write((vls_output_t){STDERR_FILENO, msg, strlen(msg)});
    return -1;
  }

  const char *not_a_file = "Not a file";
  if (!S_ISREG(file.st_mode)) {
    vls_safety_write(
        (vls_output_t){STDERR_FILENO, not_a_file, strlen(not_a_file)});
    return -1;
  }

  const char *not_md = "Cannot create hash";
  EVP_MD_CTX *contex = EVP_MD_CTX_new();
  if (contex == NULL) {
    goto hash_error;
  }

  if (EVP_DigestInit_ex(contex, EVP_md5(), NULL) != 1) {
    goto hash_error;
  }

  unsigned char buff[BUFFER_SIZE];
  ssize_t bytes_read;

  while (true) {
    bytes_read = read(fd, buff, BUFFER_SIZE);
    if (bytes_read < 0) {
      goto hash_error;
    } else if (bytes_read == 0) {
      break;
    }
    if (EVP_DigestUpdate(contex, buff, bytes_read) != 1) {
      goto hash_error;
    }
  }

  if (bytes_read < 0) {
    goto hash_error;
  }

  unsigned char md_buff[EVP_MAX_MD_SIZE];
  unsigned int len;
  EVP_DigestFinal_ex(contex, md_buff, &len);
  EVP_MD_CTX_free(contex);
  close(fd);

  memcpy(result->bytes, md_buff, MD_SIZE);
  return 0;
hash_error:
  EVP_MD_CTX_free(contex);
  vls_safety_write((vls_output_t){STDERR_FILENO, not_md, strlen(not_md)});
  close(fd);
  return -1;
}

int hash_to_string(const vls_md_hash_t *hash, char *result) {
  static const char *hex = "0123456789abcdef";

  for (int i = 0; i < MD_SIZE; i++) {
    result[i * 2] = hex[(hash->bytes[i] >> 4) & 0x0F];
    result[i * 2 + 1] = hex[hash->bytes[i] & 0x0F];
  }

  return 0;
}

int is_identical(const vls_md_hash_t *first, const vls_md_hash_t *second) {
  return memcmp(first->bytes, second->bytes, MD_SIZE);
}
