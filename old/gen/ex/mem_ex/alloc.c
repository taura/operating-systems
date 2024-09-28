#include <stdio.h>
#include <stdlib.h>

int main(int argc, char ** argv) {
  if (argc <= 1) { fprintf(stderr, "usage: %s N (MB)\n", argv[0]); exit(1); }
  unsigned long N = atol(argv[1]);
  unsigned long sz = (N * 1024L * 1024L);
  /* alloc N MB */
  char * a = malloc(sz);
  if (a == NULL) { perror("malloc"); exit(1); }
  printf("# malloc(%lu) = %p\n", sz, a);
  return 0;
}
