#ifndef __FINWO_AS_STORAGE_H__
#define __FINWO_AS_STORAGE_H__

#include "common.h"
#include "linked-list.h"

struct storage_ctx {
  char *filename;
};

struct storage_ctx * storage_init(char *directory);
JSON_Value         * storage_get(struct storage_ctx *ctx, char *key);
JSON_Status          storage_set(struct storage_ctx *ctx, char *key, JSON_Value *value);
JSON_Status          storage_del(struct storage_ctx *ctx, char *key);
void                 storage_close(struct storage_ctx *ctx);

struct storage_dirent {
  char *name;
  int is_character_device;
  int is_block_device;
  int is_directory;
  int is_fifo;
  int is_symlink;
  int is_file;
  int is_socket;
};

LLIST(struct storage_dirent, storage_dirlist);

struct storage_dirlist * storage_readdir(char *path);
void storage_dirlist_free(struct storage_dirlist *list);

#endif // __FINWO_AS_STORAGE_H__
