#include "utils/iterator.h"
#include "vls_types.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

node_t *add_next(node_t *list, void *data) {
  node_t *next = malloc(sizeof(node_t));
  if (!next)
    return list;

  if (list) {
    list->next = next;
    next->prev = list;
  }
  next->next = NULL;
  next->data = data;
  return next;
}

void list_free(node_t *list, bool free_data) {
  if (!list)
    return;
  while (list->prev)
    list = list->prev;

  while (list) {
    node_t *next = list->next;
    if (free_data && list->data)
      free(list->data);
    free(list);
    list = next;
  }
}

int iterate(execute_one execute, node_t *list, void *ctx) {
  if (!list)
    return 0;
  node_t *curr = list;
  while (curr->prev)
    curr = curr->prev;

  while (curr) {
    int out = execute(curr->data, ctx);
    if (out < 0)
      return out;
    curr = curr->next;
  }
  return 0;
}
