/* 
 * int_seq.c
 *
 * compile:
 *   gcc int_seq.c -lm
 *
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

double integrate(double (*f)(double), double a, double b, int N)
{
  int i;
  double s = 0.0;
  for (i = 0; i < N; i++) {
    double x      = a + (b - a) *  i      / N;
    double x_next = a + (b - a) * (i + 1) / N;
    double dx = x_next - x;
    s += f(x) * dx;
  }
  return s;
}


double cur_time()
{
  struct timeval tp[1];
  gettimeofday(tp, NULL);
  return tp->tv_usec * 1.0E-6 + tp->tv_sec;
}

int main()
{
  double a = 0.0;
  double b = M_PI / 2.0;
  double t0 = cur_time();
  double s = integrate(sin, a, b, 100000000);
  double t1 = cur_time();
  printf("\\int_[%f,%f] sin(x) dx = %f\n", a, b, s);
  printf("%.6f sec\n", t1 - t0);
  return 0;
}
