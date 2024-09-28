/* 
 * openmp_inc.c
 * compile this file by:
      gcc -fopenmp openmp_inc.c -lpthread
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <omp.h>

long g = 0;
void * thread_func(long n_inc, long * c, long * p) {
  long i;
  
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
    c[0]++;
  }
  for (i = 0; i < n_inc; i++) {
    p[0]++;
  }

  printf("g = %ld, s = %ld, l = %ld, c[0] = %ld, p[0] = %ld\n",
	 g, s, l, c[0], p[0]);

  return 0;
}

double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, NULL);
  return tp->tv_sec + tp->tv_usec * 1.0E-6;
}

int main(int argc, char ** argv)
{
  if (argc <= 1) {
    fprintf(stderr, 
	    "usage: OMP_NUM_THREADS=no_of_threads %s no_of_increments\n"
	    "example:\n   OMP_NUM_THREADS=16 %s 10000000\n",
	    argv[0], argv[0]);
    exit(1);
  }
  long n_inc = atol(argv[1]);
  double t0 = cur_time();
  long a_counter[1] = { 0 };

  /* スレッドを N_THREADS 個作る */
#pragma omp parallel
  {
    long c_counters[1] = { 0 };
    thread_func(n_inc, c_counters, a_counter); 
  }

  double t1 = cur_time();
  printf("OK: elapsed time: %f\n", t1 - t0);
  return 0;
}

