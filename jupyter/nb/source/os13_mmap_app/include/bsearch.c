#include <stdio.h>
#include <stdlib.h>

int compare_int(const void * a_, const void * b_) {
  const int * a = a_;
  const int * b = b_;
  return *a - *b;
}

int main(int argc, char ** argv) {
  int key = (argc > 1 ? atoi(argv[1]) : 100);
  int a[] = { 100, 200, 300, 400, 500 };
  int n = sizeof(a) / sizeof(a[0]);
  int * found = bsearch(&key, a, n, sizeof(int), compare_int);
  if (found) {
    printf("%d found at %ld-th element\n", key, found - a);
  } else {
    printf("%d not found\n", key);
  }
  return 0;
}
