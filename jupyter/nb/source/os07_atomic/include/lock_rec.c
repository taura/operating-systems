#com 3
#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>
#include <omp.h>

void die(const char * msg) {
  perror(msg); exit(1);
}

/* 大域変数 */
volatile double g;

double cur_time() {
  struct timespec ts[1];
  clock_gettime(CLOCK_REALTIME, ts);
  return ts->tv_sec + ts->tv_nsec * 1.0e-9;
}

void waste_sec(double dt) {
  if (dt > 0) {
    double t0 = cur_time();
    while (cur_time() < t0 + dt) ;
  }
}

#ifpy VER == 1
/* pthread_mutexで排他制御 */
typedef pthread_mutex_t mutex_t;
int mutex_init(mutex_t * m) {
  return pthread_mutex_init(m, 0);
}
int mutex_lock(mutex_t * m) {
  return pthread_mutex_lock(m);
}
int mutex_unlock(mutex_t * m) {
  return pthread_mutex_unlock(m);
}
#elifpy VER == 2
/* pthread_spin_lockで排他制御 */
typedef pthread_spinlock_t mutex_t;
int mutex_init(mutex_t * m) {
  return pthread_spin_init(m, 0);
}
int mutex_lock(mutex_t * m) {
  return pthread_spin_lock(m);
}
int mutex_unlock(mutex_t * m) {
  return pthread_spin_unlock(m);
}
#elsepy
/* 排他制御なし(以下はダミー) */
typedef int mutex_t;
int mutex_init(mutex_t * m) { *m = 0; return 0; }
int mutex_lock(mutex_t * m) { (void)m; return 0; }
int mutex_unlock(mutex_t * m) { (void)m; return 0; }
#endifpy

/* x = ax + b を l 回行う(特に意味のない計算) */
double compute(double a, double x, double b, long l) {
  for (long i = 0; i < l; i++) {
    x = a * x + b;
  }
  return x;
}

typedef struct {
  double t[2];
} double2_t;

#ifpy VER <= 2
double2_t atomic_update(double a, volatile double * p, double b, long l, mutex_t * m) {
  mutex_lock(m);
  double t1 = cur_time();
  *p = compute(a, *p, b, l);
  double t2 = cur_time();
  mutex_unlock(m);
  double2_t t = {{t1, t2}};
  return t;
}

#elsepy
int compare_and_swap_double(volatile double * p, double x, double y) {
  union { long l; double d; } o, n;
  o.d = x;
  n.d = y;
  return __sync_bool_compare_and_swap((long *)p, o.l, n.l);
}

double2_t atomic_update(double a, volatile double * p, double b, long l, mutex_t * m) {
  (void)m;
  while (1) {
    double t1 = cur_time();
    double x = *p;
    double y = compute(a, x, b, l);
    assert(x != y);
    if (compare_and_swap_double(p, x, y)) {
      double t2 = cur_time();
      double2_t t = {{t1, t2}};
      return t;
    }
  }
  assert(0);
}
#endifpy

/* 記録 */
typedef struct {
  int thread;                   /* スレッド番号 */
  int cpu;                      /* CPU番号 */
  double lock_enter;            /* lockを呼んだ時刻 */
  double lock_acq;              /* lockがreturnした(獲得した)時刻 */
  double unlock_enter;          /* unlockを呼んだ時刻 */
} record_t;

int main(int argc, char ** argv) {
  int i = 1;
  /* スレッド数 */
  long nthreads = (argc > i ? atol(argv[i]) : 4);    i++;
  /* (合計)更新数 */
  long n        = (argc > i ? atol(argv[i]) : 100000); i++;
  /* 1回の更新あたりの時間 (x = ax+b の回数)
     クリティカルセクションの長さを調節 */
  long l        = (argc > i ? atol(argv[i]) : 1000); i++;
  /* クリティカルセクション外の時間(秒) */
  double time_out = (argc > i ? atof(argv[i]) : 0);    i++;

  double a = (argc > i ? atof(argv[i]) : 1.0 - 1.0e-10); i++;
  double b = (argc > i ? atof(argv[i]) : 1.0); i++;
  
  g = 0.0;
  mutex_t m[1];
  mutex_init(m);
  record_t * rec = (record_t *)calloc(sizeof(record_t) * n, 1);
  
#pragma omp parallel num_threads(nthreads) 
  {
    int idx = omp_get_thread_num();
#pragma omp for schedule(static)
    for (long i = 0; i < n; i++) {
      double t0 = cur_time();
      double2_t t12 = atomic_update(a, &g, b, l, m);
      waste_sec(time_out);
      rec[i].thread = idx;
      rec[i].cpu = sched_getcpu();
      rec[i].lock_enter = t0;
      rec[i].lock_acq = t12.t[0];
      rec[i].unlock_enter = t12.t[1];
    }
  }
  printf("g = %lf\n", g);
  for (long i = 0; i < n; i++) {
    printf("%ld %d %d %.9f %.9f %.9f\n", i, rec[i].thread, rec[i].cpu,
           rec[i].lock_enter, rec[i].lock_acq, rec[i].unlock_enter);
  }
  return 0;
}
