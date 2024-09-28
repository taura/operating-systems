#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>

void record_faults(FILE * wp, int x) {
  static double t0 = 0.0;
  struct timeval tp[1];
  struct rusage u[1];
  /* get current time */
  if (gettimeofday(tp, NULL) == -1) { perror("gettimeofday"); exit(1); }
  /* get minor/major faults */
  if (getrusage(RUSAGE_SELF, u) == -1) { perror("getrusage"); exit(1); }
  double t = tp->tv_sec + tp->tv_usec * 1.0E-6;
  if (t0 == 0.0) { t0 = t; }
  fprintf(wp, "%d %f %ld %ld\n", x, t - t0, u->ru_minflt, u->ru_majflt);
}

int main(int argc, char ** argv) {
  if (argc <= 1) { fprintf(stderr, "usage: %s N (MB)\n", argv[0]); exit(1); }
  unsigned long N = atol(argv[1]);
  unsigned long sz = (N * 1024L * 1024L);
  /* alloc N MB */
  char * a = malloc(sz);
  if (a == NULL) { perror("malloc"); exit(1); }
  //printf("# malloc(%lu) = %p\n", sz, a);
  unsigned long i,j;
  unsigned long p = 0;
  /* scan N MB three times */
  for (i = 0; i < 3; i++) {
    //printf("# %ld-th scan\n", i);
    for (j = 0; j < sz; j += 4096) {
      if (p % 1024 == 0) record_faults(stdout, p);
      a[j]++;			/* touch the page */
      p++;			/* count acceses */
    }
  }
  return 0;
}
