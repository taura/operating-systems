#com 9
#include <assert.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#ifpy VER == 7
#include <x86intrin.h>
#elifpy VER == 8
#include <errno.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <sys/time.h>

int futex(int * uaddr, int futex_op, int val) {
  return syscall(SYS_futex, uaddr, futex_op, val, 0, 0, 0);
}
#endifpy

/* 大域変数 */
#ifpy VER == 9
int g = 0;
#elsepy
volatile int g = 0;
#endifpy

#ifpy VER == 1
/* スレッドの開始関数 */
void * f(void * arg_) {
  long * arg = arg_;
  long n = arg[0];
  for (long i = 0; i < n; i++) {
    g++;
  }
  return 0;
}
#endifpy

#ifpy VER == 6
long atomic_add_cas(volatile int * p, int x) {
  while (1) {
    int r = *p;
    if (__sync_bool_compare_and_swap(p, r, r + x)) {
      return r;
    }
  }
  assert(0);
}
#elifpy VER == 7

long atomic_add_tm(volatile int * p, int x, pthread_mutex_t * m) {
  for (long failed = 0; failed < 10; failed++) {
    if (_xbegin() == _XBEGIN_STARTED) {
      if (m->__data.__lock) _xabort(1);
      int r = *p;
      *p = r + x;
      _xend();
      return r;
    }
  }
  pthread_mutex_lock(m);
  int r = *p;
  *p = r + x;
  pthread_mutex_unlock(m);
  return r;
}

#elifpy VER == 8

typedef struct {
  int locked;
} lock_t;

int lock_init(lock_t * l) {
  l->locked = 0;
  return 0;
}

int lock(lock_t * l) {
  while (1) {
    int locked = l->locked;
    if (locked == 0) {
      if (__sync_bool_compare_and_swap(&l->locked, 0, 1)) {
        return 0;
      }
    } else {
      if (futex(&l->locked, FUTEX_WAIT, 1) == -1 && errno != EAGAIN)
        err(1, "futex");
    }
  }
}

int unlock(lock_t * l) {
  l->locked = 0;
  if (futex(&l->locked, FUTEX_WAKE, 1) == -1) 
    err(1, "futex");
  return 0;
}
#endifpy

int main(int argc, char ** argv) {
  long n = (argc > 1 ? atol(argv[1]) : 1000000);
#ifpy VER == 1
  long arg[1] = { n };
#endifpy
  g = 0;
#ifpy VER == 1
  /* スレッドを作る */
  const int nthreads = 2;
  pthread_t child_thread_id[nthreads];
  for (int i = 0; i < nthreads; i++) {
    if (pthread_create(&child_thread_id[i], 0, f, arg))
      err(1, "pthread_create");
  }
  /* 終了待ち */
  for (int i = 0; i < nthreads; i++) {
    void * ret = 0;
    if (pthread_join(child_thread_id[i], &ret))
      err(1, "pthread_join");
  }
#elsepy
#ifpy VER == 4 or VER == 7
  pthread_mutex_t m[1];
  pthread_mutex_init(m, 0);
#elifpy VER == 8
  lock_t l[1];
  lock_init(l);
#endifpy
#pragma omp parallel
  {
#ifpy VER >= 3
#pragma omp for
#endifpy
    for (long i = 0; i < n; i++) {
#ifpy VER < 4
      g++;
#elifpy VER == 4
      pthread_mutex_lock(m);
      g++;
      pthread_mutex_unlock(m);
#elifpy VER == 5
      __sync_fetch_and_add(&g, 1);
#elifpy VER == 6
      atomic_add_cas(&g, 1);
#elifpy VER == 7
      atomic_add_tm(&g, 1, m);
#elifpy VER == 8
      lock(l);
      g++;
      unlock(l);
#elifpy VER == 9
      __transaction_atomic {
        g++;
      }
#endifpy
    }
  }
#endifpy
  printf("g = %d\n", g);
  return 0;
}
