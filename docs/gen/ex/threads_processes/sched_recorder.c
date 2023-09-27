/* 
 * recorder.c
 */

#define _GNU_SOURCE
#include <utmpx.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <time.h>
#include <unistd.h>

typedef struct rec {
  double a;
  double b;
  int proc;
} * rec_t;

double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, NULL);
  return tp->tv_sec + tp->tv_usec * 1.0E-6;
}

void die(char * s) {
  perror(s);
  exit(1);
}

int main(int argc, char ** argv) {
  double T = 10.0;
  int n = 100000;
  if (argc > 1) T = atof(argv[1]);
  if (argc > 2) n = atoi(argv[2]);
  pid_t pid = getpid();
  double limit = cur_time() + T;
  rec_t R = (rec_t)calloc(n, sizeof(struct rec));
  int i = 0;
  /* R[i] : R[i].a -- R[i].b まで動いていた記録 */
  R[i].a = R[i].b = cur_time();
  R[i].proc = sched_getcpu();
  if (argc > 3) sleep(1);
  while (R[i].b < limit && i < n) {
    double next_time = cur_time(); /* 現在時刻を得る */
    int cpu = sched_getcpu();
    if (next_time - R[i].b < 1.0E-3 && cpu == R[i].proc) {
      /* 最後に見た時刻とあまり変わらない(< 1ms) -> R[i].bを増やす */
      R[i].b = next_time;
    } else {
      /* 最後に見た時刻から1ms以上たっている -> 新しい区間に入る */
      if (i + 1 >= n) break;
      i++;
      R[i].proc = cpu;
      if (0) 
	printf("%d was not running %f - %f\n", 
	       pid, R[i-1].b, next_time);
      R[i].a = R[i].b = cur_time();
    }
  }
  assert(i < n);
  double dt = limit - R[i].b;
  if (dt > 0.0) {
    struct timespec req[1];
    req->tv_sec = (time_t)dt;
    req->tv_nsec = (long)((dt - req->tv_sec) * 1.0E9);
    if (nanosleep(req, NULL) == -1) die("nanosleep:");
  }
  int j;
  for (j = 0; j <= i; j++) {
    printf("%d %f %f %d %f\n", 
	   pid, R[j].a, R[j].b, R[j].proc,
	   R[j].b - R[j].a);
  }
  return 0;
}
