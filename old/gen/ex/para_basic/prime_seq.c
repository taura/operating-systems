/* 
 * prime_seq.c
 *
 *
 */

#include <stdio.h>
#include <sys/time.h>

int check_prime(int n)
{
  int d;
  for (d = 2; d * d <= n; d++) {
    if (n % d == 0) return 0;
  }
#if 0
  printf("%d is prime\n", n);
#endif
  return 1;
}

int check_primes(int a, int b)
{
  int n;
  int s = 0;
  for (n = a; n < b; n++) {
    s += check_prime(n);
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
  int N = 10000000;
  double t0 = cur_time();
  int np = check_primes(2, N); 
  double t1 = cur_time();
  printf("%d primes in %f sec\n", np, t1 - t0);
  return 0;
}

