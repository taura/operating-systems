#com 2
#ifpy VER % 2 == 1
/* 必要な #include を補うこと (man ページを参照) */
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#elsepy
#include <err.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#endifpy

long cur_time() {
  struct timespec ts[1];
  clock_gettime(CLOCK_REALTIME, ts);
  return ts->tv_sec * 1000000000L + ts->tv_nsec;
}

void * do_nothing(void * arg) {
  return arg;
}

int main(int argc, char ** argv) {
  int n = (argc > 1 ? atoi(argv[1]) : 5);
  long t0 = cur_time();
#ifpy VER % 2 == 1

  
  /* ここにプログラムを書く */

  
#elsepy VER % 2 == 0
  for (int i = 0; i < n; i++) {
    pthread_t tid;
    void * ret;
    int e = pthread_create(&tid, 0, do_nothing, 0);
    if (e) err(e, "pthread_create");
    e = pthread_join(tid, &ret);
    if (e) err(e, "pthread_join");
  }
#endifpy
  long t1 = cur_time();
  long dt = t1 - t0;
  printf("%ld nsec to pthrea_create-and-join %d threads (%ld nsec/thread)\n",
         dt, n, dt / n);
  return 0;
}
