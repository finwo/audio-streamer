#ifndef __FINWO_AS_STORAGE_H__
#define __FINWO_AS_STORAGE_H__

#include "common.h"

struct storage_ctx {
  char *filename;
};

struct storage_ctx * storage_init(char *directory);
JSON_Value         * storage_get(struct storage_ctx *ctx, char *key);
void                 storage_set(struct storage_ctx *ctx, char *key, JSON_Value *value);
void                 storage_del(struct storage_ctx *ctx, char *key);
void                 storage_close(struct storage_ctx *ctx);

#endif // __FINWO_AS_STORAGE_H__
