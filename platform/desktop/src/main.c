#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kgabis/parson.h"
#include "webview/webview.h"

#include "common.h"
#include "storage.h"

#ifdef _WIN32
#include <Windows.h>
#endif

static const char html[] = {
#include "../docroot/manager.h"
};

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

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif


  // Initialize context we pass to bindings and such
  struct as_ctx *ctx = calloc(1, sizeof(struct as_ctx));

  // Load up our main storage location
  const char *storageLocationRelative = "/.config/finwo/audio-streamer";
  char *storageLocation = calloc(strlen(getenv("HOME")) + strlen(storageLocationRelative) + 1, 1);
  strcat(storageLocation, getenv("HOME"));
  strcat(storageLocation, storageLocationRelative);
  ctx->storage_ctx = storage_init(storageLocation);

  // Initialize the window
  ctx->w = webview_create(0, NULL);
  webview_set_title(ctx->w, "Audio streamer");
  webview_set_size(ctx->w, 800, 600, WEBVIEW_HINT_NONE);
  webview_set_html(ctx->w, html);

  // Initialize bindings
  webview_bind(ctx->w, "wv_title_set", fn_set_title, ctx);
  webview_bind(ctx->w, "storage_get", fn_storage_get, ctx);
  webview_bind(ctx->w, "storage_set", fn_storage_set, ctx);
  webview_bind(ctx->w, "storage_del", fn_storage_del, ctx);

  // And start the thing
  webview_run(ctx->w);
  webview_destroy(ctx->w);

  // Properly close storage
  storage_close(ctx->storage_ctx);

  return 0;
}
