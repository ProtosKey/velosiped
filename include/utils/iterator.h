#ifndef ITERATOR_H
#define ITERATOR_H

#include "vls_types.h"
#include <stdbool.h>

typedef int (*execute_one)(void *, void *);
node_t *add_next(node_t *, void *);
int iterate(execute_one, node_t *, void *);
void list_free(node_t *, bool);
bool is_clear_list(node_t *);
bool check_is_in(node_t *, const char *);

#endif
