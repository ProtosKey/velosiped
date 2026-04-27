#ifndef ITERATOR_H
#define ITERATOR_H

#include "vls_types.h"

typedef int (*execute_one)(void *, void *);
int iterate(execute_one, node_t *);

#endif
