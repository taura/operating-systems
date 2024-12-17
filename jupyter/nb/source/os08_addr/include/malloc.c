/*** com 5 */
#include <stdio.h>
#include <stdlib.h>
/*** if VER >= 4 */
#include <gc/gc.h>
/*** endif */

/*** if VER == 1 */
int main(int argc, char ** argv) {
  long n = (argc > 1 ? atol(argv[1]) : 3);
  if (n > 3000) {
    fprintf(stderr, "n (%ld) too large (> 3000)\n", n);
  }
  printf("allocating 100MB %ld times\n");
  for (long i = 0; i < n; i++) {
    void * a = malloc(100 * 1000 * 1000);
    printf("a@main() :     %ld\n", a);
  }
}
/*** elif VER == 2 */
int main(int argc, char ** argv) {
  long n = (argc > 1 ? atol(argv[1]) : 10);
  long repeat = (argc > 1 ? atol(argv[1]) : 5);
  if (n > 3000) {
    fprintf(stderr, "n (%ld) too large (> 3000)\n", n);
  }
  printf("allocate 100MB blocks %ld x %ld times, retaining %ld blocks\n",
         n, repeat, n);
  void * a[n];
  for (long r = 0; r < n; r++) {
    for (long i = 0; i < n; i++) {
      if (r > 0) free(a[i]);
      a[i] = malloc(100 * 1000 * 1000);
      printf("a@main() :     %ld\n", a[i]);
    }
  }
}
/*** elif VER == 3 */
#include <unistd.h>
int main(int argc, char ** argv) {
  long n = (argc > 1 ? atol(argv[1]) : 3);
  if (n > 3000) {
    fprintf(stderr, "n (%ld) too large (> 3000)\n", n);
  }
  printf("allocating 100MB %ld times\n");
  for (long i = 0; i < n; i++) {
    void * a = sbrk(100 * 1000 * 1000);
    printf("a@main() :     %ld\n", a);
  }
}
/*** elif VER == 4 */
int main(int argc, char ** argv) {
  long n = (argc > 1 ? atol(argv[1]) : 3);
  if (n > 3000) {
    fprintf(stderr, "n (%ld) too large (> 3000)\n", n);
  }
  printf("allocating 100MB %ld times\n");
  for (long i = 0; i < n; i++) {
    void * a = GC_MALLOC(100 * 1000 * 1000);
    printf("a@main() :     %ld\n", a);
  }
}
/*** elif VER == 5 */
int main(int argc, char ** argv) {
  long n = (argc > 1 ? atol(argv[1]) : 10);
  long repeat = (argc > 1 ? atol(argv[1]) : 5);
  if (n > 3000) {
    fprintf(stderr, "n (%ld) too large (> 3000)\n", n);
  }
  printf("allocate 100MB blocks %ld x %ld times, retaining %ld blocks\n",
         n, repeat, n);
  void * A[n];
  for (long r = 0; r < n; r++) {
    for (long i = 0; i < n; i++) {
      A[i] = GC_MALLOC(100 * 1000 * 1000);
      printf("a@main() :     %ld\n", A[i]);
    }
  }
}
/*** endif */
