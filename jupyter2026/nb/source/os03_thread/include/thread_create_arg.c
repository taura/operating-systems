#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

/* 開始関数に渡したい情報(構造体) */
typedef struct {
  long slp_usec;
  long slp_n;
  long id;                       /* 0,1,2,.. */
  pthread_t th_id;
} thread_arg_t;

/* 開始関数 構造体へのポインタを (void *型で)受け取る */
void * f(void * arg_) {
  thread_arg_t * arg = arg_;
  /* 本当に受け取りたい引数を構造体から受け取る */
  long slp_usec = arg->slp_usec;
  long slp_n = arg->slp_n;
  long id = arg->id;
  pthread_t thread_id = pthread_self();
  for (int i = 0; i < slp_n; i++) {
    printf("child[%ld/%lu] (%d/%ld): sleep %ld usec\n",
           id, thread_id, i, slp_n, slp_usec);
    fflush(stdout);
    usleep(slp_usec);
  }
  return 0;
}

int main(int argc, char ** argv) {
  int nthreads = (argc > 1 ? atoi(argv[1]) : 3);
  thread_arg_t args[nthreads];
  /* 指定された数のスレッドを作る */
  for (int i = 0; i < nthreads; i++) {
    args[i].slp_n = i + 2;
    args[i].slp_usec = 1000 * 1000 / args[i].slp_n;
    args[i].id = i;
    if (pthread_create(&args[i].th_id, 0, f, &args[i])) {
      err(1, "pthread_create");
    }
  }
  /* 終了待ち */
  for (int i = 0; i < nthreads; i++) {
    void * ret;
    if (pthread_join(args[i].th_id, &ret)) {
      err(1, "pthread_join");
    }
    assert(ret == 0);
    printf("child thread %d returned (%p)\n", i, ret);
  }  
  return 0;
}
