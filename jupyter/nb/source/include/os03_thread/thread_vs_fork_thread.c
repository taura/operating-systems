#com 1
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* スレッドの開始関数 */
void * f(void * arg_) {
  int * arg = arg_;
  *arg = 123;                   /* i.e., x = 123 */
  return 0;
}

int main() {
  pthread_t child_thread_id;
  int x = 10;
  
  /* スレッドを作る */
  if (pthread_create(&child_thread_id, 0, f, &x)) {
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
