#include <pthread.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "webview/webview.h"

#include "bound.h"
#include "common.h"
#include "storage.h"

#include "task/http.h"

#include "../docroot/manager.h"

#ifdef _WIN32
#include <Windows.h>
#endif

struct as_ctx *ctx = NULL;

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif
  pthread_t tid_http;

  // Initialize context we pass to bindings and such
  // Shared across all threads
  ctx = calloc(1, sizeof(struct as_ctx));

  // Load up our main storage location
  const char *storageLocationRelative = "/.config/finwo/audio-streamer";
  char *storageLocation = calloc(strlen(getenv("HOME")) + strlen(storageLocationRelative) + 1, 1);
  strcat(storageLocation, getenv("HOME"));
  strcat(storageLocation, storageLocationRelative);
  ctx->storage_ctx = storage_init(storageLocation);

  // Start all tasks
  if (pthread_create(&tid_http, NULL, task_http, ctx)) { perror("Starting http task"); return 1; }

  // Initialize the window
  ctx->w = webview_create(1, NULL);
  webview_set_title(ctx->w, "Audio streamer");
  webview_set_size(ctx->w, 800, 600, WEBVIEW_HINT_NONE);
  webview_set_html(ctx->w, doc_manager);

  // Initialize bindings
  struct bound_fn_llist *bound_fn = bound_fns;
  while(bound_fn) {
    webview_bind(ctx->w, bound_fn->data->name, bound_fn->data->fn, ctx);
    bound_fn = bound_fn->next;
  }

  // And start the thing
  webview_run(ctx->w); // Only exits when window is closed
  webview_destroy(ctx->w); // Cleanup

  // Wait for all tasks to finish
  pthread_join(tid_http, NULL);

  // Properly close storage
  storage_close(ctx->storage_ctx);

  return 0;
}
