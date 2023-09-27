#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <time.h>

/* 多項式 a[0] z^{n-1} + a[1] z^{n-2} + ... + a[n-1] 
   を表すデータ構造 */
typedef struct complex_fun_param {
  int n;
  double complex * a;
} complex_fun_param;

/* Aを左下隅，Cを右上隅とする長方形を表すデータ構造 

   +------+ C
   |      |
   |      |
   +------+
  A
*/
typedef struct contour_param {
  double complex A;
  double complex C;
} contour_param;

/* 色々な多項式を色々な長方形の周上で複素線積分する */

typedef double complex (*contour_t)(contour_param *, double);
typedef double complex (*complex_fun_t)(complex_fun_param *, double complex);

/* 
   曲線 C : z(t) (t : a -> b) 
   に沿った f(z) の線積分 ∫C f(z) dz 

   ∫[a,b] f(z(t)) * z'(t) dt

   
   
 */

double complex contour_integral(complex_fun_t f, complex_fun_param * fa, 
				contour_t z, contour_param * za,
				double a, double b, long n) {
  long i;
  double dt = (b - a) / n;
  double complex s = 0.0;
  for (i = 0; i < n; i++) {
    double t = a + (i * (b - a)) / n;
    s += f(fa, z(za, t)) * (z(za, t + dt) - z(za, t - dt)) * 0.5;
  }
  return s;
}

/* 正方形のパラメータ表示. (t : 0 -> 4 で一周) 

         C
   +--<--+
   |     |
   |     |
   +-->--+
  A
*/
double complex square(contour_param * p, double t) {
  while (t < 0.0) t += 4.0;
  while (t >= 4.0) t -= 4.0;
  assert(0 <= t);
  assert(t < 4.0);
  /* 
     D --<-- C
     |       |
     A -->-- B
 */
  double complex A = p->A;
  double complex C = p->C;
  double complex B = creal(C) + cimag(A) * 1.0I;
  double complex D = creal(A) + cimag(C) * 1.0I;

  if (0 <= t && t < 1) return A + (B - A) * t;
  if (1 <= t && t < 2) return B + (C - B) * (t - 1);
  if (2 <= t && t < 3) return C + (D - C) * (t - 2);
  if (3 <= t && t < 4) return D + (A - D) * (t - 3);
  assert(0);
}

/* 多項式f(z) = a[0] * z^{n-1} + a[1] * z^{n-2} + ... + a[n-1]
   に対し， f'(z) / f(z) を計算 */
double complex poly_arg(complex_fun_param * p, double complex z) {
  int n = p->n;
  double complex s = 0.0, t = 0.0;
  int i;
  /* f(z) */
  double complex zi = 1.0;
  for (i = 0; i < n; i++) {
    s += p->a[n - i - 1] * zi;
    zi *= z;
  }
  /* f'(z) */
  double complex zi_1 = 1.0;
  for (i = 1; i < n; i++) {
    t += i * p->a[n - i - 1] * zi_1;
    zi_1 *= z;
  }
  return t / s;
}

double cur_time() {
  struct timespec ts[1];
  clock_gettime(CLOCK_REALTIME, ts);
  return ts->tv_sec + ts->tv_nsec * 1.0e-9;
}



int main(int argc, char ** argv) {
  long n = (argc > 1 ? atol(argv[1]) : 1000000);
  /* z^6 - 1 */
  enum { degree = 6 };
  double complex coeff[degree+1] = { 1, 0, 0, 0, 0, 0, -1 };
  complex_fun_param p[1] = { { degree + 1, coeff } };

  /* 以下の正方形を m x m に分割し，
     できる100個の小さな正方形のまわりで線積分

                C
        +-------+
        |       |
        |       | h
        |       |
        +-------+
        A   w 
   */

  double complex A = -2.0 - 2.0I, C = 2.0 + 2.0I;
  double w = creal(C) - creal(A);
  double h = cimag(C) - cimag(A);
  int m = 5;

  double t0 = cur_time();
  int i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < m; j++) {
      double complex P = A + i * w / m + (j * h / m) * 1.0I;
      double complex Q = P +     w / m + (    h / m) * 1.0I;
      contour_param cp[1] = { { P, Q } } ;
      double complex r = contour_integral(poly_arg, p, square, cp, 0, 4.0, n);
      printf("contour integral of f'(z)/f(z) along square [%.1f,%.1f]x[%.1f,%.1f] = %.9f + %.9f i\n",
	     creal(P), creal(Q), cimag(P), cimag(Q), creal(r), cimag(r));
    }
  }
  double t1 = cur_time();
  printf("elapsed time : %.9f sec\n", t1 - t0);
  return 0;
}
