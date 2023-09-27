/* 
 * pthread_inc.c
 * compile this file by:
      gcc pthread_inc.c -lpthread
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

typedef struct thread_arg {
  pthread_t tid;
  int idx;
  long n_inc;
  long c[1];
  long *p;
} * thread_arg_t;

long g = 0;
void * thread_func(void * _arg) {
  thread_arg_t arg = _arg;
  long i;
  long n_inc = arg->n_inc;
  
  static long s = 0;
  long l = 0;
  for (i = 0; i < n_inc; i++) {
    g++;
  }
  for (i = 0; i < n_inc; i++) {
    s++;
  }
  for (i = 0; i < n_inc; i++) {
    l++;
  }
  for (i = 0; i < n_inc; i++) {
    arg->c[0]++;
  }
  for (i = 0; i < n_inc; i++) {
    arg->p[0]++;
  }

  printf("g = %ld, s = %ld, l = %ld, c[0] = %ld, p[0] = %ld\n",
	 g, s, l, arg->c[0], arg->p[0]);

  return 0;
}

double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, NULL);
  return tp->tv_sec + tp->tv_usec * 1.0E-6;
}

int main(int argc, char ** argv)
{
  if (argc <= 2) {
    fprintf(stderr, 
	    "usage: %s no_of_threads no_of_increments\n"
	    "example:\n   %s 16 10000000\n",
	    argv[0], argv[0]);
    exit(1);
  }
  int n_threads = atoi(argv[1]);
  long n_inc = atol(argv[2]);

  struct thread_arg args[n_threads];
  double t0 = cur_time();
  long a_counter[1] = { 0 };
  int i;

  /* スレッドを N_THREADS 個作る */
  for (i = 0; i < n_threads; i++) {
    args[i].idx = i;
    args[i].n_inc = n_inc;
    args[i].c[0] = 0;
    args[i].p = a_counter;
    pthread_create(&args[i].tid, NULL, 
		   thread_func, (void *)&args[i]);
  }
  /* 終了待ち */
  for (i = 0; i < n_threads; i++) {
    pthread_join(args[i].tid, NULL);
  }
  double t1 = cur_time();
  printf("OK: elapsed time: %f\n", t1 - t0);
  return 0;
}

