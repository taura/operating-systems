/*** com 9 */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
/*** if VER > 1 */
#include <omp.h>
/*** endif */

int check_prime(long n) {
  for (long d = 2; d * d <= n; d++) {
    if (n % d == 0) return 0;
  }
  return n > 1;
}

/*** if VER == 1 */
void count_primes(long a, long b, long * s) {
  for (long n = a; n < b; n++) {
    if (check_prime(n)) {
      *s += 1;
    }
  }
}
/*** elif VER == 2 */
void count_primes(long a, long b, long * s) {
#pragma omp for
  for (long n = a; n < b; n++) {
    if (check_prime(n)) {
      *s += 1;
    }
  }
}
/*** elif VER == 3 */
void count_primes(long a, long b, long * s, pthread_mutex_t * m) {
#pragma omp for
  for (long n = a; n < b; n++) {
    if (check_prime(n)) {
      pthread_mutex_lock(m);
      *s += 1;
      pthread_mutex_unlock(m);
    }
  }
}
/*** else */
/* 以下のstruct, 関数の中身を埋めよ */
typedef struct {
/*** if VER == 5 */
  long x;
  pthread_mutex_t m[1];
/*** elif VER == 7 */
  long x;
/*** elif VER == 9 */
  long x;
/*** endif */
} counter_t;

void counter_init(counter_t * c) {
/*** if VER == 5 */
  c->x = 0;
  pthread_mutex_init(c->m, 0);
/*** elif VER == 7 */
  c->x = 0;
/*** elif VER == 9 */
  c->x = 0;
/*** else  */
  /* 0 にする 
     (void)cは変数を使っていないという警告を消すためのもの.
     修正後は消して良い */
  (void)c;
/*** endif */
}

long counter_inc(counter_t * c) {
  /* +1 する (返り値: 深い意味はなく, 元の値を返すとする) */
/*** if VER == 5 */
  pthread_mutex_lock(c->m);
  long x = c->x;
  c->x = x + 1;
  pthread_mutex_unlock(c->m);
  return x;
/*** elif VER == 7 */
  return __sync_fetch_and_add (&c->x, 1);
/*** elif VER == 9 */
  while (1) {
    long x = c->x;
    if (__sync_bool_compare_and_swap(&c->x, x, x + 1)) {
      return x;
    }
  }
/*** else  */
  (void)c;
  return -1;
/*** endif */
}

long counter_get(counter_t * c) {
/*** if VER == 5 */
  return c->x;
/*** elif VER == 7 */
  return c->x;
/*** lif VER == 9 */
  return c->x;
/*** else  */
  /* 今の値を返す */
  (void)c;
  return -1;
/*** endif */
}

void count_primes(long a, long b, counter_t * c) {
#pragma omp for
  for (long n = a; n < b; n++) {
    if (check_prime(n)) {
/*** if VER == 5 */
      counter_inc(c);
/*** endif */
    }
  }
}
/*** endif */

/*** if VER == 1 */
typedef struct {
  pthread_t tid;
  long idx;
  long nthreads;
  long a;
  long b;
  long * s;
} count_prime_args_t;

/* スレッドの開始関数 */
void * count_primes_fun(void * arg_) {
  count_prime_args_t * arg = arg_;
  long d = arg->b - arg->a;
  long my_a = arg->a + d *  arg->idx      / arg->nthreads;
  long my_b = arg->a + d * (arg->idx + 1) / arg->nthreads;
  count_primes(my_a, my_b, arg->s);
  return 0;
}
/*** endif */

double cur_time() {
  struct timespec ts[1];
  clock_gettime(CLOCK_REALTIME, ts);
  return ts->tv_nsec * 1.0E-9 + ts->tv_sec;
}

int main(int argc, char ** argv) {
  long i = 1;
  long a = (argc > i ? atol(argv[i]) : 1000000); i++;
  long b = (argc > i ? atol(argv[i]) : 2000000); i++;
/*** if VER == 1 */
  long nthreads = (argc > i ? atol(argv[i]) : 4); i++;
  long s = 0;
  count_prime_args_t args[nthreads];
  double t0 = cur_time();
  /* スレッドを作る */
  for (long idx = 0; idx < nthreads; idx++) {
    args[idx].idx = idx;
    args[idx].nthreads = nthreads;
    args[idx].a = a;
    args[idx].b = b;
    args[idx].s = &s;
    if (pthread_create(&args[idx].tid, 0, count_primes_fun, &args[idx])) {
      err(1, "pthread_create");
    }
  }
  /* 終了待ち */
  for (long idx = 0; idx < nthreads; idx++) {
    void * ret = 0;
    if (pthread_join(args[idx].tid, &ret)) {
      err(1, "pthread_join");
    }
  }
  double t1 = cur_time();
/*** else */
/*** if VER <= 3 */
  long s = 0;
/*** if VER == 3 */
  pthread_mutex_t m[1];
  pthread_mutex_init(m, 0);
/*** endif */
/*** else */
  counter_t c[1];
  counter_init(c);
/*** endif */
  double t0 = cur_time();
#pragma omp parallel            
  {
    /* 起動時に環境変数OMP_NUM_THREADS=xxx で指定した
       個数のスレッドが作られ, 各々が以下の文 { ... }
       を実行する.
       関数内のpragma omp for 下のfor文をそれらのスレッドが
       分割して実行する */
/*** if VER <= 2 */
    count_primes(a, b, &s);
/*** elif VER == 3 */
    count_primes(a, b, &s, m);
/*** else */
    count_primes(a, b, c);
/*** endif */
  }
  double t1 = cur_time();
/*** endif */
/*** if VER <= 3 */
  printf("%ld primes in [%ld,%ld)\n", s, a, b);
/*** else */
  printf("%ld primes in [%ld,%ld)\n", counter_get(c), a, b);
/*** endif */
  printf("%f sec\n", t1 - t0);
  return 0;
}
