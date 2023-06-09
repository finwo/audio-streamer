#include <dirent.h>
#include <libgen.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <wordexp.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/sysmacros.h>

#include "kgabis/parson.h"

#include "common.h"
#include "storage.h"

const char *relpath = "data.json";

// ---[ Utility functions ]---

// Origin: https://stackoverflow.com/a/2336245
// Modified to support longer paths (including linux's limit of 4096)
void mkdirp(char *dir) {
  char *tmp  = strdup(dir);
  size_t len = strlen(tmp);
  char *p    = NULL;

  // Remove trailing slash
  if (tmp[len - 1] == '/') {
    tmp[len - 1] = 0;
    len--;
  }

  // Run mkdir for all parent directories
  // Toggles '/' to 0 and back to generate sub-paths
  for (p = tmp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0;
      mkdir(tmp, S_IRWXU);
      *p = '/';
    }
  }

  // And finally the directory itself
  mkdir(tmp, S_IRWXU);
  free(tmp);
}

// ---[ JSON CONFIG ]---

struct storage_ctx * storage_init(char *directory) {
  struct storage_ctx *ctx = malloc(sizeof(struct storage_ctx));
  ctx->filename = calloc(strlen(directory) + 1 + strlen(relpath) + 1, 1);
  strcat(ctx->filename, directory);
  strcat(ctx->filename, "/");
  strcat(ctx->filename, relpath);
  return ctx;
}

JSON_Value * storage_get(struct storage_ctx *ctx, char *key) {
  JSON_Value *root_value = json_parse_file(ctx->filename);
  JSON_Value *result;
  if (root_value == NULL) return json_value_init_null();
  result = json_object_get_value(json_value_get_object(root_value), key);
  result = json_value_deep_copy(result);
  json_value_free(root_value);
  if (result == NULL) return json_value_init_null();
  return result;
}

JSON_Status storage_set(struct storage_ctx *ctx, char *key, JSON_Value *value) {
  JSON_Value *root_value = json_parse_file_with_comments(ctx->filename);
  if (root_value == NULL) root_value = json_value_init_object();
  JSON_Status status = json_object_set_value(json_object(root_value), key, json_value_deep_copy(value));

  // Ensure the directory exists
  char *pathdup   = strdup(ctx->filename);
  char *directory = dirname(pathdup);
  mkdirp(directory);
  free(pathdup);

  status = json_serialize_to_file(root_value, ctx->filename);
  json_value_free(root_value);
  return status;
}

JSON_Status storage_del(struct storage_ctx *ctx, char *key) {
  JSON_Value *root_value = json_parse_file(ctx->filename);
  if (root_value == NULL) return JSONSuccess;
  json_object_remove(json_value_get_object(root_value), key);

  // Ensure the directory exists
  char *pathdup   = strdup(ctx->filename);
  char *directory = dirname(pathdup);
  mkdirp(directory);
  free(pathdup);

  JSON_Status status = json_serialize_to_file(root_value, ctx->filename);
  json_value_free(root_value);
  return status;
}

void storage_close(struct storage_ctx *ctx) {
  // Nothing to do
}

// ---[ Library management ]---


struct storage_dirlist * storage_readdir(char *path) {
  struct storage_dirlist *result = NULL;
  struct storage_dirlist *next   = NULL;
  wordexp_t p;
  char **entry;
  char *ipath;
  char *rpath;
  int i;

  DIR *dp;
  struct dirent *dent;
  struct stat   estat;

  // Expand & store all results
  wordexp(path, &p, WRDE_NOCMD);
  entry = p.we_wordv;
  for (i = 0; i < p.we_wordc; i++) {

    // Read the directory we expanded to
    dp = opendir(entry[i]);
    while((dent = readdir(dp)) != NULL) {

      // Build the full name of the entry
      ipath = calloc(1, strlen(entry[i]) + 1 + strlen(dent->d_name) + 1);
      strcat(ipath, entry[i]);
      strcat(ipath, "/");
      strcat(ipath, dent->d_name);
      rpath = realpath(ipath, NULL);
      free(ipath);

      // Stat, so we can populate our entry
      if (stat(rpath, &estat) < 0) {
        free(rpath);
        continue;
      }

      // Build a new return entry
      next                   = result;
      result                 = malloc(sizeof(struct storage_dirlist));
      result->next           = next;
      result->data           = calloc(1, sizeof(struct storage_dirent));
      result->data->name     = strdup(dent->d_name);
      result->data->fullpath = rpath;

      // Copy st_mode in a more transferrable way
      switch (estat.st_mode & S_IFMT) {
        case S_IFBLK:  result->data->is_block_device     = 1; break;
        case S_IFCHR:  result->data->is_character_device = 1; break;
        case S_IFDIR:  result->data->is_directory        = 1; break;
        case S_IFIFO:  result->data->is_fifo             = 1; break;
        case S_IFLNK:  result->data->is_symlink          = 1; break;
        case S_IFREG:  result->data->is_file             = 1; break;
        case S_IFSOCK: result->data->is_socket           = 1; break;
        default:       /* intentionally empty */              break;
      }
    }
    closedir(dp);
  }
  wordfree(&p);

  return result;
}

void storage_dirlist_free(struct storage_dirlist *list) {
  if (!list) return;

  // Free data if set
  if (list->data) {
    if (list->data->name    ) free(list->data->name    );
    if (list->data->fullpath) free(list->data->fullpath);
    free(list->data);
  }

  // Free the next entry if set
  if (list->next) storage_dirlist_free(list->next);

  // Free itself
  free(list);
}
