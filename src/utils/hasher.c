#include "utils/hasher.h"
#include "utils/input_output.h"
#include "utils/logger.h"
#include "vls_types.h"
#include <errno.h>
#include <fcntl.h>
#include <openssl/evp.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int hash_my_path_and_bytes(const char *path, const void *extra,
                           size_t extra_len, vls_md_hash_t *result) {
  int fd = open(path, O_RDONLY);
  if (fd < 0)
    return vls_report_errno_at(path, errno);

  struct stat file;
  if (stat(path, &file) < 0) {
    close(fd);
    return vls_report_errno_at(path, errno);
  }

  if (!S_ISREG(file.st_mode)) {
    close(fd);
    return vls_report("Not a file");
  }

  EVP_MD_CTX *contex = EVP_MD_CTX_new();
  if (contex == NULL) {
    close(fd);
    return vls_report("Cannot create hash");
  }

  if (EVP_DigestInit_ex(contex, EVP_md5(), NULL) != 1)
    goto hash_error;

  unsigned char buff[BUFFER_SIZE];
  while (true) {
    ssize_t bytes_read = read(fd, buff, BUFFER_SIZE);
    if (bytes_read < 0)
      goto hash_error;
    if (bytes_read == 0)
      break;
    if (EVP_DigestUpdate(contex, buff, bytes_read) != 1)
      goto hash_error;
  }

  if (extra_len > 0 &&
      EVP_DigestUpdate(contex, extra, extra_len) != 1)
    goto hash_error;

  unsigned char md_buff[EVP_MAX_MD_SIZE];
  unsigned int len;
  EVP_DigestFinal_ex(contex, md_buff, &len);
  EVP_MD_CTX_free(contex);
  close(fd);

  memcpy(result->bytes, md_buff, MD_SIZE);
  return 0;

hash_error:
  EVP_MD_CTX_free(contex);
  close(fd);
  return vls_report("Cannot create hash");
}

int hash_my_path(const char *path, vls_md_hash_t *result) {
  return hash_my_path_and_bytes(path, NULL, 0, result);
}

int hash_to_string(const vls_md_hash_t *hash, char *result) {
  static const char *hex = "0123456789abcdef";
  for (int i = 0; i < MD_SIZE; i++) {
    result[i * 2] = hex[(hash->bytes[i] >> 4) & 0x0F];
    result[i * 2 + 1] = hex[hash->bytes[i] & 0x0F];
  }
  result[MD_SIZE * 2] = '\0';
  return 0;
}

int hash_from_string(char *string, vls_md_hash_t *result) {
  if (!string || !result)
    return -1;

  for (int i = 0; i < MD_SIZE; i++) {
    unsigned char high, low;

    char c1 = string[i * 2];
    if (c1 >= '0' && c1 <= '9')
      high = c1 - '0';
    else if (c1 >= 'a' && c1 <= 'f')
      high = c1 - 'a' + 10;
    else if (c1 >= 'A' && c1 <= 'F')
      high = c1 - 'A' + 10;
    else
      return -1;

    char c2 = string[i * 2 + 1];
    if (c2 >= '0' && c2 <= '9')
      low = c2 - '0';
    else if (c2 >= 'a' && c2 <= 'f')
      low = c2 - 'a' + 10;
    else if (c2 >= 'A' && c2 <= 'F')
      low = c2 - 'A' + 10;
    else
      return -1;
    result->bytes[i] = (high << 4) | low;
  }

  return 0;
}

int is_identical(const vls_md_hash_t *first, const vls_md_hash_t *second) {
  return memcmp(first->bytes, second->bytes, MD_SIZE);
}
