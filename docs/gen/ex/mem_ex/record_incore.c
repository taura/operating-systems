#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <unistd.h>

void record_incore(FILE * wp, 
		     void * addr, size_t length,
		     void * last_accessed) {
  static unsigned char * R = 0;
  static size_t n_pages = 0;
  static double t0 = 0.0;
  struct timeval tp[1];
  if (gettimeofday(tp, NULL) == -1) { perror("gettimeofday"); exit(1); }
  double t = tp->tv_sec + tp->tv_usec * 1.0e-6;
  const int page_size = 4096;

  if (R == 0) {			/* this is the first time */
    n_pages = (length + page_size - 1) / page_size;
    R = calloc(n_pages, 1);
    t0 = t;
  }
  /* align addr to page boundary */
  addr = (void*)(((int64_t)addr) & ~4095L);
  if (mincore(addr, length, R) == -1) {
    perror("mincore"); exit(1);
  } else {
    size_t i;
    unsigned char c = 0;
    size_t x = 0;
    /* get current time */
    fprintf(wp, "%f", t - t0);
    fprintf(wp, " %lu", (last_accessed - addr) / page_size);
    for (i = 0; i < n_pages; i++) {
      if (R[i] != c) {
	fprintf(wp, " %lu", x);
	c = R[i];
	x = 0;
      }
      x++;
    }
    assert(x > 0);
    fprintf(wp, " %lu", x);
    fprintf(wp, "\n");
  }
}

int main(int argc, char ** argv) {
  if (argc <= 1) { fprintf(stderr, "usage: %s N (MB)\n", argv[0]); exit(1); }
  unsigned long N = atol(argv[1]);
  unsigned long sz = (N * 1024L * 1024L);
  /* alloc N MB */
  //char * a = malloc(sz);
  //if (a == NULL) { perror("malloc"); exit(1); }
  char * a = sbrk(sz);
  if (a == NULL) { perror("sbrk"); exit(1); }
  //printf("# malloc(%lu) = %p\n", sz, a);
  unsigned long i,j;
  unsigned long p = 0;
  /* scan N MB three times */
  for (i = 0; i < 3; i++) {
    //printf("# %ld-th scan\n", i);
    for (j = 0; j < sz; j += 4096) {
      if (p % 1024 == 0) record_incore(stdout, a, sz, &a[j]);
      a[j]++;			/* touch the page */
      p++;			/* count acceses */
    }
  }
  return 0;
}
