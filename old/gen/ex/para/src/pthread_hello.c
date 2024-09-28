/* 
 * pthread_hello.c
 * compile this file by:
      gcc pthread_hello.c -lpthread
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#define N_THREADS 16

typedef struct thread_arg {
  pthread_t tid;
  int idx;
} * thread_arg_t;

void * thread_func(void * _arg) {
  thread_arg_t arg = _arg;
  int i;
  for (i = 0; i < 10; i++) {
    printf("thread %d says %d-th greetings\n", arg->idx, i);
    usleep(1000);		/* 1ms */
  }
  return 0;
}

double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, NULL);
  return tp->tv_sec + tp->tv_usec * 1.0E-6;
}

int main()
{
  struct thread_arg args[N_THREADS];
  double t0 = cur_time();
  int i;
  /* スレッドを N_THREADS 個作る */
  for (i = 0; i < N_THREADS; i++) {
    args[i].idx = i;
    pthread_create(&args[i].tid, NULL, 
		   thread_func, (void *)&args[i]);
  }
  /* 終了待ち */
  for (i = 0; i < N_THREADS; i++) {
    pthread_join(args[i].tid, NULL);
  }
  double t1 = cur_time();
  printf("OK: elapsed time: %f\n", t1 - t0);
  return 0;
}

