/* 
 * openmp_hello.c
 * compile this file by:
      gcc -fopenmp openmp_hello.c 
 */
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

void * thread_func() {
  int id = omp_get_thread_num();
  int i;
  for (i = 0; i < 10; i++) {
    printf("thread %d says %d-th greetings\n", id, i);
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
  double t0 = cur_time();
#pragma omp parallel
  {
    thread_func();
  }
  double t1 = cur_time();
  printf("OK: elapsed time: %f\n", t1 - t0);
  return 0;
}

