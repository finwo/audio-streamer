#ifndef __FINWO_AS_COMMON_H__
#define __FINWO_AS_COMMON_H__

#include "webview/webview.h"

struct as_ctx {
  webview_t w;
  void *storage_ctx;
};

#endif // __FINWO_AS_COMMON_H__
