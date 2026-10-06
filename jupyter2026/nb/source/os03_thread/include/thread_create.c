#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* スレッドの開始関数 */
void * f(void * arg) {
  pthread_t thread_id = pthread_self();
  int slp_n = 5;
  for (int i = 0; i < slp_n; i++) {
    printf("child[%lu]: (%d/%d)\n",
           thread_id, i, slp_n);
    fflush(stdout);
    usleep(100 * 1000);
  }
  return arg + 1;
}

int main() {
  pthread_t my_thread_id = pthread_self();
  pthread_t child_thread_id;
  /* スレッドを作る */
  if (pthread_create(&child_thread_id, 0, f, 0)) {
    err(1, "pthread_create");
  }
  int slp_n = 5;
  for (int i = 0; i < slp_n; i++) {
    printf("parent[%lu]: (%d/%d)\n", my_thread_id, i, slp_n);
    fflush(stdout);
    usleep(100 * 1000);
  }
  /* 終了待ち */
  void * ret = 0;
  if (pthread_join(child_thread_id, &ret)) {
    err(1, "pthread_join");
  }
  printf("child thread returned %p\n", ret);
  return 0;
}
