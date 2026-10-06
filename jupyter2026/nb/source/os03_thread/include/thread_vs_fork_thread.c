#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int x = 0;

/* スレッドの開始関数 */
void * f(void * arg) {
  x += 321;
  return 0;
}

int main() {
  pthread_t child_thread_id;
  x = 123;
  
  /* スレッドを作る */
  if (pthread_create(&child_thread_id, 0, f, 0)) {
    err(1, "pthread_create");
  }
  /* 終了待ち */
  void * ret = 0;
  if (pthread_join(child_thread_id, &ret)) {
    err(1, "pthread_join");
  }
  printf("after the child finished, x = %d\n", x);
  return 0;
}
