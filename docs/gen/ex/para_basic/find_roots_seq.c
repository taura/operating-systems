/* 
 * find_roots_seq.c
 */

#include <assert.h>
#include <math.h>
#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#define DBG 0

const double delta = 1.0E-8;

/* #define DBG 1 */

/* --------------------------------------------
   struct polynomial
   data structure representing a polynomial.
   --------------------------------------------

   e.g., 4 + 2z + z^2
   4 + 2z + z^2 is represented by

   n = 3   (number of terms. or degree + 1)
   a[0] = 4
   a[1] = 2
   a[2] = 1
   -------------------------------------------- */
typedef struct polynomial
{
  int n;			/* degree + 1 */
  double _Complex *a; 		/* array of coefficients */
} * polynomial_t;

/* make a polynomial */
polynomial_t mk_polynomial(int n, double _Complex *a)
{
  polynomial_t f = (polynomial_t)malloc(sizeof(struct polynomial));
  assert(f);
  f->n = n;
  f->a = a;
  return f;
}

/* given f(z), return f'(z) */
polynomial_t differentiate(polynomial_t f)
{
  int m = f->n - 1;
  double _Complex * a = (double _Complex *)malloc(sizeof(double _Complex) * m);
  assert(a);
  int i;
  for (i = 0; i < m; i++) {
    a[i] = f->a[i + 1] * (i + 1);
  }
  return mk_polynomial(m, a);
}

/* evaluate f(z) for a given polynomial f */
double _Complex eval_poly(polynomial_t f, double _Complex z)
{
  int i;
  if (z == 0.0) {
    return f->a[0];
  } else if (cabs(z) > 1) {
    /* for large z, add from low order terms */
    double _Complex s = 0.0;
    double _Complex z_i = 1.0;
    for (i = 0; i < f->n; i++) {
      s += f->a[i] * z_i;
      z_i = z_i *z;
    }
    return s;
  } else {
    /* for small z, add from high order terms */
    double _Complex s = 0.0;
    double _Complex r = 1.0 / z;
    double _Complex r_i = 1.0;
    for (i = f->n - 1; i >= 0; i--) {
      s += f->a[i] * r_i;
      r_i = r_i * r;
    }
    return s / r_i;
  }
}


/* --------------------------------------------
   struct ratinal 
   data structure representing a rational (polynomial / polynomial).
   --------------------------------------------
   a over b.
   a : numerator (bunshi)
   b : denominator (bunbo)
   -------------------------------------------- */
typedef struct rational
{
  /* a / b */
  polynomial_t a;		
  polynomial_t b;
} * rational_t;

/* make a rational */
rational_t mk_rational(polynomial_t a, polynomial_t b)
{
  rational_t q = (rational_t)malloc(sizeof(struct rational));
  assert(q);
  q->a = a;
  q->b = b;
  return q;
}

/* evaluate q(z) for a given rational q. */
double _Complex eval_ratio(rational_t q, double _Complex z)
{
  return eval_poly(q->a, z) / eval_poly(q->b, z);
}

/* --------------------------------------------
   struct complex_array
   data structure to which we store found roots 
   -------------------------------------------- */
typedef struct complex_array
{
  int n;
  int sz;
  double _Complex * a;
} * complex_array_t;

/* make an empty array */
complex_array_t mk_complex_array(int sz)
{
  complex_array_t ca = (complex_array_t)malloc(sizeof(struct complex_array));
  assert(ca);
  ca->sz = sz;
  ca->n = 0;
  ca->a = (double _Complex *)malloc(sizeof(double _Complex) * sz);
  assert(ca->a);
  return ca;
}

/* add an element (complex number) z into array ca */
void complex_array_add(complex_array_t ca, double _Complex z)
{
  int n = ca->n;
  int i;
  for (i = 0; i < n; i++) {
    if (cabs(ca->a[i] - z) < 1.1 * delta) return;
  } 
  if (n < ca->sz) {
#if DBG
    printf("root found: %.9f+%.9fi\n", creal(z), cimag(z));
#endif
    ca->a[n] = z;
    ca->n = n + 1;
  } else {
#if DBG
    printf("root found but too many!! %.9f+%.9fi\n", creal(z), cimag(z));
#endif
    assert(n < ca->sz);
  }
}

/* --------------------------------------------
   integral function
   --------------------------------------------

  a+iL   C2      a+L+iL
    +------<-----+
    |            |
    |            |
 C3 |            ^ C1
    |            |
    |            |
    +---->-------+
   a      C0      a+L


   calculate line integral of q for the above contour C0 + C1 + C2 + C3.

 */

double _Complex integrate(rational_t q, double _Complex a, double L)
{
  int i;
  double _Complex z;
  int N = 1.0E3 * L * L;
  if (N < 128) N = 128;

  /* C0 */
  double _Complex s0 = 0.0;
  z = a;
  for (i = 0; i < N; i++) {
    double _Complex z_ = a + (i + 1) * L / N;
    s0 += eval_ratio(q, z) * (z_ - z);
    z = z_;
  }

  /* C1 */
  double _Complex s1 = 0.0;
  z = a + L;
  for (i = 0; i < N; i++) {
    double _Complex z_ = a + L + I * (i + 1) * L / N;
    s1 += eval_ratio(q, z) * (z_ - z);
    z = z_;
  }

  /* C2 */
  double _Complex s2 = 0.0;
  z = a + L + I * L;
  for (i = 0; i < N; i++) {
    double _Complex z_ = a + L + I * L - (i + 1) * L / N;
    s2 += eval_ratio(q, z) * (z_ - z);
    z = z_;
  }

  /* C3 */
  double _Complex s3 = 0.0;
  z = a + I * L;
  for (i = 0; i < N; i++) {
    double _Complex z_ = a + I * L - I * (i + 1) * L / N;
    s3 += eval_ratio(q, z) * (z_ - z);
    z = z_;
  }

  return s0 + s1 + s2 + s3;
}



/* Given rational function q(z) and the following square region, 

      L
   +------+
   |      |
   |      | L
   +------+
   a  

   judge if the following complex line integral is zero.

          /
         |
         |  q(z) dz
         |
        / C

   C is the curve surrounding the square.

   If it is not zero, we divide the square into four subsquares
   and try to find which subsquares give non-zero values.

   We continue this until the size of the square becomes so small.
 */

void find_zeros_rec(rational_t r, double _Complex a, double L, complex_array_t ca)
{
  double _Complex s = integrate(r, a, L);
  double m = creal(s / (2.0 * M_PI * I));
  int mi = (int)floor(m + 0.4);
  if (mi > 0) {
    /*  
	+-----------+
	|           |
	|           |
      a3+-----+a2   |
	|     |     |
	|     |     |
        +-----+-----+
     a=a0     a1   

    */

    if (L < delta) {
#if DBG
      printf("** L=%.9f, a=%.9f+%.9fi s=%.9f+%.9fi=2pi*i(%.9f+%.9fi)\n", 
	     L, creal(a), cimag(a), creal(s), cimag(s),
	     creal(s / (2.0 * M_PI * I)), cimag(s / (2.0 * M_PI * I)));
#endif
      complex_array_add(ca, a);
    } else {
      double L2 = L * 0.5;
      double _Complex a0 = a;
      double _Complex a1 = a0 + L2;
      double _Complex a2 = a1 + I * L2;
      double _Complex a3 = a0 + I * L2;
      find_zeros_rec(r, a2, L2, ca);
      find_zeros_rec(r, a3, L2, ca);
      find_zeros_rec(r, a0, L2, ca);
      find_zeros_rec(r, a1, L2, ca);
    }
  }
}



/* --------------------------------------------
   find roots of a polynomial f
   -------------------------------------------- 
   
   We do this by the following fact.

   Consider the following square region and let C 
   be the boundary of it.

   +------+
   |      |
   |      | C
   +------+

   The line integral

   /  
   |   f'(z)
   | -------- dz
   |   f(z)
   / C

   is zero if and only if the square does not contain any root 
   of f (in fact, the above integral is the sum of multiplicities
   of f's roots in the region).

   Hence, given a square region, if we find that the integral
   of f'(z)/f(z) through the boundary of the square is zero,
   we know that the region does not contain any root of f.

   This way, we narrow the regions containing any root of f.

   -------------------------------------------- */

complex_array_t find_roots(polynomial_t f)
{
  complex_array_t ca = mk_complex_array(f->n - 1);
  int i;
  /* make f'(z) */
  polynomial_t f_prime = differentiate(f);
  /* make r(z) = f'(z)/f(z) */
  rational_t r = mk_rational(f_prime, f);
  /* make bounding box */
  double R = 0.0;
  for (i = 0; i < f->n - 1; i++) {
    R += cabs(f->a[i]);
  }
  R /= f->a[f->n - 1];
  find_zeros_rec(r, -R-I*R, 2.1 * R, ca);
  return ca;
}

double cur_time()
{
  struct timeval tp[1];
  gettimeofday(tp, NULL);
  return tp->tv_sec + tp->tv_usec * 1.0E-6;
}

int main()
{
  double _Complex a[] = { 1, 1, 1, 1, 1, 1  }; /* 1 + z + ... + z^5 */
  int n = sizeof(a) / sizeof(a[0]);
  polynomial_t f = mk_polynomial(n, a);
  double t0 = cur_time();
  complex_array_t ca = find_roots(f);
  double t1 = cur_time();
  int i; 
  printf("%d roots found in %f sec\n", ca->n, t1 - t0);
  for (i = 0; i < ca->n; i++) {
    printf("%+f%+fi\n",  creal(ca->a[i]), cimag(ca->a[i]));
  }
  return 0;
}

