#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kgabis/parson.h"

#include "common.h"
#include "storage.h"

const char *relpath = "data.json";

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

void storage_set(struct storage_ctx *ctx, char *key, JSON_Value *value) {
  JSON_Value *root_value = json_parse_file_with_comments(ctx->filename);
  if (root_value == NULL) root_value = json_value_init_object();
  JSON_Status status = json_object_set_value(json_object(root_value), key, json_value_deep_copy(value));
  json_serialize_to_file(root_value, ctx->filename);
  json_value_free(root_value);
}

void storage_del(struct storage_ctx *ctx, char *key) {
  JSON_Value *root_value = json_parse_file(ctx->filename);
  if (root_value == NULL) return;
  json_object_remove(json_value_get_object(root_value), key);
  json_serialize_to_file(root_value, ctx->filename);
  json_value_free(root_value);
}

void storage_close(struct storage_ctx *ctx) {
  // Nothing to do
}
