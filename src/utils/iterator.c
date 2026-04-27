#include "vls_types.h"
#include <stdbool.h>

typedef int (*execute_one)(void *, void *);
int iterate(execute_one execute, node_t list, void *ctx) {
  int out;
  while (true) {
    if ((out = execute(list.data, ctx)) < 0) {
      return out;
    }
    if (!list.next) {
      return 0;
    }
  }
}
