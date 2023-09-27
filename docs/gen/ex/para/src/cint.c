#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <complex.h>
#include <time.h>

/* 実数 -> 複素数 (曲線のパラメータ表示) の型 countour_t */
typedef double complex (*contour_t)(double);
/* 複素数 -> 複素数の型 complex_fun_t */
typedef double complex (*complex_fun_t)(double complex);

/* 
   曲線 C : z(t) (t : a -> b) 
   に沿った f(z) の線積分 ∫C f(z) dz 

   ∫[a,b] f(z(t)) * z'(t) dt
   
 */

double complex contour_integral(complex_fun_t f, contour_t z, 
				double a, double b, long n) {
  long i;
  double dt = (b - a) / n;
  double complex s = 0.0;
  for (i = 0; i < n; i++) {
    double t = a + (i * (b - a)) / n;
    s += f(z(t)) * (z(t + dt) - z(t - dt)) * 0.5;
  }
  return s;
}

/* 単位円のパラメータ表示. (t : 0 -> 2pi で一周) */
double complex unit_circle(double t) {
  return cexp(I * t);
}

/* 正方形のパラメータ表示. (t : 0 -> 4 で一周) 

         (1,1)
   +--<--+
   |     |
   |     |
   +-->--+
(-1,-1)
*/
double complex square(double t) {
  while (t < 0.0) t += 4.0;
  while (t >= 4.0) t -= 4.0;
  assert(0 <= t);
  assert(t < 4.0);
  if (0 <= t && t < 1) return (-1.0 - 1.0I) + 2.0  * t;
  if (1 <= t && t < 2) return ( 1.0 - 1.0I) + 2.0I * (t - 1);
  if (2 <= t && t < 3) return ( 1.0 + 1.0I) - 2.0  * (t - 2);
  if (3 <= t && t < 4) return (-1.0 + 1.0I) - 2.0I * (t - 3);
  assert(0);
}

/* f(z) = 1/z */
double complex cinv(double complex z) {
  return 1.0 / z;
}

double cur_time() {
  struct timespec ts[1];
  clock_gettime(CLOCK_REALTIME, ts);
  return ts->tv_sec + ts->tv_nsec * 1.0e-9;
}

int main(int argc, char ** argv) {
  long n = (argc > 1 ? atol(argv[1]) : 10000000);
  double t0 = cur_time();
  double complex s = contour_integral(cinv, unit_square, 0, 4.0, n);
  double t1 = cur_time();
  printf("contour integral of 1/z along square [-1,1]x[-1,1] = %.15f + %.15f i\n",
	 creal(s), cimag(s));
  printf("elapsed time : %.9f sec\n", t1 - t0);
  return 0;
}
