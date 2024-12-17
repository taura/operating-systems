/*** com 5 */
#include <stdio.h>

/*** if VER == 1 */
int main() {
  /* 局所変数 */
  int x;
  /* 局所配列 */
  int c[100];
  double d[200];
  /* 局所変数 */
  int i;
  printf("x:     %ld\n", &x);
  printf("c:     %ld\n", c);
  printf("c[50]: %ld\n", &c[50]);
  printf("d:     %ld\n", d);
  printf("d[50]: %ld\n", &d[50]);
  printf("i:     %ld\n", &i);
}
/*** elif VER == 2 */
void g() {
  int x = 20;
  printf("x@g : %ld = %d\n", &x, x);
}

void f() {
  int x = 10;
  g();
  printf("x@f : %ld = %d\n", &x, x);
}

int main() {
  f();
  return 0;
}
/*** elif VER == 3 */
#include <stdlib.h>
int g;
long fact(long n) {
  printf("g : %ld\n", (long)&g);
  printf("n@fact(%ld) : %ld\n", n, (long)&n);
  if (n == 0) {
    return 1;
  } else {
    long r = fact(n - 1);
    printf("r@fact(%ld) : %ld\n", n, (long)&r);
    return n * r;
  }
}

int main(int argc, char ** argv) {
  long n = (argc > 1 ? atol(argv[1]) : 10);
  long x = fact(n);
  printf("fact(%ld) = %ld\n", n, x);
  return 0;
}
/*** elif VER == 4 */
long fib(long n) {
  printf("n@fib(%ld) : %ld\n", n, (long)&n);
  if (n < 2) {
    return 1;
  } else {
    long x = fib(n - 1);
    long y = fib(n - 2);
    printf("x@fib(%ld) : %ld\n", n, (long)&x);
    printf("y@fib(%ld) : %ld\n", n, (long)&y);
    return x + y;
  }
}

int main() {
  long n = 10;
  long x = fib(n);
  printf("fib(%ld) = %ld\n", n, x);
  return 0;
}
/*** elif VER == 5 */
double * f(double * a) {
  return a;
}
const int n = 30;
double * alloc() {
  double a[n];
  return f(a);
}

int main() {
  double * a = alloc();
  for (long i = 0; i < n; i++) {
    a[i] = i;
  }
  for (long i = 0; i < n; i++) {
    printf("a[%ld] = %f\n", i, a[i]);
  }
  return 0;
}
/*** endif */
