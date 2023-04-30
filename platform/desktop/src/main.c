#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "kgabis/parson.h"
#include "webview/webview.h"

#ifdef _WIN32
#include <Windows.h>
#endif

static const char html[] = {
#include "../docroot/manager.h"
};

struct as_ctx {
  webview_t w;
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


#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInt, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
#else
int main() {
#endif

  // Initialize context we pass to bindings and such
  struct as_ctx *ctx = calloc(1, sizeof(struct as_ctx));

  // Initialize the window
  ctx->w = webview_create(0, NULL);
  webview_set_title(ctx->w, "Audio streamer");
  webview_set_size(ctx->w, 480, 320, WEBVIEW_HINT_NONE);
  webview_set_html(ctx->w, html);

  // Initialize bindings
  webview_bind(ctx->w, "wv_title_set", fn_set_title, ctx);

  // And start the thing
  webview_run(ctx->w);
  webview_destroy(ctx->w);

  return 0;
}
