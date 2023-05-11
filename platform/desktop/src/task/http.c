#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "http.h"

void * task_http(void *arg) {
  printf("Hello from the http thread!\n");
  pthread_exit(NULL);
}
