#ifndef __FINWO_AS_BOUND_H__
#define __FINWO_AS_BOUND_H__

#include <stdlib.h>

#include "linked-list.h"

struct bound_fn {
  char *name;
  void (*fn)(const char *seq, const char *req, void *arg);
};

LLIST(struct bound_fn, bound_fn_llist);

extern struct bound_fn_llist *bound_fns;

#endif // __FINWO_AS_BOUND_H__
