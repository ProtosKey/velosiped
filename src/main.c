#include <unistd.h>

int main() {
  ssize_t n = write(1, "Hello, World!\n", 14);
  if (n < 0) {
    return 1;
  }
  return 0;
}
