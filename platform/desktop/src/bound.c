#include "kgabis/parson.h"
#include "webview/webview.h"

#include "bound.h"
#include "storage.h"

struct bound_fn_llist *bound_fns = NULL;

void fn_set_title(const char *seq, const char *req, void *udata) {
  struct as_ctx *ctx = udata;
  JSON_Array *jArguments;
  const char *newTitle;

  JSON_Value *jreq = json_parse_string(req);
  if (json_value_get_type(jreq) != JSONArray) {
    json_value_free(jreq);
    webview_return(ctx->w, seq, 0, "new Error(\"Invalid bound call\")");
    return;
  }

  // Extract the string
  jArguments = json_value_get_array(jreq);
  newTitle   = json_array_get_string(jArguments, 0);
  webview_set_title(ctx->w, newTitle);

  // Close down our method
  json_value_free(jreq);
  webview_return(ctx->w, seq, 0, "undefined");
}

void fn_storage_get(const char *seq, const char *req, void *udata) {
  struct as_ctx *ctx = udata;
  JSON_Array *jArguments;
  JSON_Value *value;
  char *key;
  char *response;

  JSON_Value *jreq = json_parse_string(req);
  if (json_value_get_type(jreq) != JSONArray) {
    json_value_free(jreq);
    webview_return(ctx->w, seq, 1, "new Error(\"Invalid bound call\")");
    return;
  }

  jArguments = json_value_get_array(jreq);
  key        = json_array_get_string(jArguments, 0);
  value      = storage_get(ctx->storage_ctx, key);
  response   = json_serialize_to_string(value);
  json_value_free(jreq);
  json_value_free(value);
  webview_return(ctx->w, seq, 0, response);
  free(response);
}

void fn_storage_set(const char *seq, const char *req, void *udata) {
  struct as_ctx *ctx = udata;
  JSON_Array *jArguments;
  JSON_Value *value;
  const char *key;

  JSON_Value *jreq = json_parse_string(req);
  if (json_value_get_type(jreq) != JSONArray) {
    json_value_free(jreq);
    webview_return(ctx->w, seq, 1, "new Error(\"Invalid bound call\")");
    return;
  }

  jArguments = json_value_get_array(jreq);
  key        = json_array_get_string(jArguments, 0);
  value      = json_array_get_value(jArguments, 1);

  if (storage_set(ctx->storage_ctx, key, value) < 0) {
    json_value_free(jreq);
    webview_return(ctx->w, seq, 1, "new Error(\"Unable to update storage\")");
    return;
  }

  json_value_free(jreq);
  webview_return(ctx->w, seq, 0, "null");
}

void fn_storage_del(const char *seq, const char *req, void *udata) {
  struct as_ctx *ctx = udata;
  JSON_Array *jArguments;
  const char *key;

  JSON_Value *jreq = json_parse_string(req);
  if (json_value_get_type(jreq) != JSONArray) {
    json_value_free(jreq);
    webview_return(ctx->w, seq, 0, "new Error(\"Invalid bound call\")");
    return;
  }

  jArguments = json_value_get_array(jreq);
  key        = json_array_get_string(jArguments, 0);

  if (storage_del(ctx->storage_ctx, key) < 0) {
    json_value_free(jreq);
    webview_return(ctx->w, seq, 1, "new Error(\"Unable to update storage\")");
    return;
  }

  json_value_free(jreq);
  webview_return(ctx->w, seq, 0, "null");
}

void fn_storage_readdir(const char *seq, const char *req, void *udata) {
  struct as_ctx *ctx = udata;
  JSON_Value *jRoot = json_value_init_array();
  JSON_Value *jEntry;
  JSON_Object *jEntryObject;
  JSON_Array *jArguments;
  char *givenpath;

  JSON_Value *jreq = json_parse_string(req);
  if (json_value_get_type(jreq) != JSONArray) {
    json_value_free(jreq);
    json_value_free(jRoot);
    webview_return(ctx->w, seq, 1, "new Error(\"Invalid bound call\")");
    return;
  }

  jArguments = json_value_get_array(jreq);
  givenpath  = json_array_get_string(jArguments, 0);

  if (!givenpath) {
    json_value_free(jreq);
    json_value_free(jRoot);
    webview_return(ctx->w, seq, 1, "new Error(\"Invalid given path\")");
    return;
  }

  struct storage_dirlist *list = storage_readdir(givenpath);
  struct storage_dirlist *entry = list;
  while(entry) {
    // Build json object
    jEntry       = json_value_init_object();
    jEntryObject = json_object(jEntry);
    json_object_set_string (jEntryObject, "name"               , entry->data->name               );
    json_object_set_string (jEntryObject, "fullpath"           , entry->data->fullpath           );
    json_object_set_boolean(jEntryObject, "is_character_device", entry->data->is_character_device);
    json_object_set_boolean(jEntryObject, "is_block_device"    , entry->data->is_block_device    );
    json_object_set_boolean(jEntryObject, "is_directory"       , entry->data->is_directory       );
    json_object_set_boolean(jEntryObject, "is_fifo"            , entry->data->is_fifo            );
    json_object_set_boolean(jEntryObject, "is_symlink"         , entry->data->is_symlink         );
    json_object_set_boolean(jEntryObject, "is_file"            , entry->data->is_file            );
    json_object_set_boolean(jEntryObject, "is_socket"          , entry->data->is_socket          );
    // And add it to the response
    json_array_append_value(json_array(jRoot), jEntry);
    entry = entry->next;
  }
  storage_dirlist_free(list);

  char *serialized = json_serialize_to_string(jRoot);
  webview_return(ctx->w, seq, 0, serialized);
  json_free_serialized_string(serialized);

  json_value_free(jreq);
  json_value_free(jRoot);
  return;

}

void bind_fn(
  char *name,
  void (*fn)(const char *seq, const char *req, void *arg)
) {
  struct bound_fn_llist *list = malloc(sizeof(struct bound_fn_llist));
  list->next = bound_fns;
  list->data = malloc(sizeof(struct bound_fn));
  list->data->name = name;
  list->data->fn   = fn;
  bound_fns = list;
}

void __attribute__ ((constructor)) register_bound_functions() {
  bind_fn("wv_title_set"    , fn_set_title      );
  bind_fn("storage_get"     , fn_storage_get    );
  bind_fn("storage_set"     , fn_storage_set    );
  bind_fn("storage_del"     , fn_storage_del    );
  bind_fn("storage_readdir" , fn_storage_readdir);
}
