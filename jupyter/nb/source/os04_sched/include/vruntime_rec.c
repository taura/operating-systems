#define _GNU_SOURCE
#include <assert.h>
#include <sched.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* 時刻 begin -- end まで proc 上で動いていた記録 */
typedef struct {
  double begin;
  double end;
  double vruntime;
  int proc;
} rec_t;

/* 現在時刻を得る */
double cur_time() {
  struct timespec tp[1];
  clock_gettime(CLOCK_REALTIME, tp);
  return tp->tv_sec + tp->tv_nsec * 1.0e-9;
}

/*** if VER == 2 */
void sleep_sec(double slp_t) {
  struct timespec tp[1];
  tp->tv_sec = (long)slp_t;
  tp->tv_nsec = (slp_t - (long)slp_t) * 1.0e9;
  nanosleep(tp, 0);
}
/*** endif */

double cur_vruntime() {
  char buf[100];
  char field[100];
  double val;
  FILE * fp = fopen("/proc/self/sched", "rb");
  char * r;
  int x;
  r = fgets(buf, sizeof(buf), fp);
  assert(r);
  r = fgets(buf, sizeof(buf), fp);
  assert(r);
  assert(strcmp("-------------------------------------------------------------------\n",
		buf) == 0);
  fseek(fp, 68, SEEK_CUR);
  r = fgets(buf, sizeof(buf), fp);
  if (!r) {
    perror("fgets"); 
    fclose(fp);
    exit(1);
  }
  fclose(fp);
  //printf("%s", buf);
  x = sscanf(buf, "%s : %lf\n", field, &val);
  assert(x == 2);
  assert(strcmp(field, "se.vruntime") == 0);
  //printf("%s=%f\n", field, val);
  return val;
}

/* T秒間走り続け, vruntimeの変化を記録する */
int run(double T,
/*** if VER == 2 */
        double run_t, double slp_t,
/*** endif */
        long n) {
  pid_t pid = getpid();
  double limit = cur_time() + T;
  rec_t * R = (rec_t *)calloc(n, sizeof(rec_t));
  long i = 0;
/*** if VER == 1 */
  R[i].begin = R[i].end = cur_time();
/*** elif VER == 2 */
  sleep_sec(slp_t);
  double start = R[i].begin = R[i].end = cur_time();
/*** endif */
  R[i].vruntime = cur_vruntime();
  R[i].proc = sched_getcpu();
  while (R[i].end < limit && i < n) {
    double t = cur_time(); /* 現在時刻を得る */
    double vr = cur_vruntime();
    int proc = sched_getcpu();
    if (vr == R[i].vruntime && proc == R[i].proc) {
      /* 最後に見たvruntimeと変化なし */
      R[i].end = t;
      R[i].vruntime = vr;
    } else {
      /* vruntimeが変化している -> 新しい区間に入る */
      if (i + 1 >= n) break;
      i++;
      R[i].proc = proc;
      R[i].begin = R[i].end = cur_time();
      R[i].vruntime = cur_vruntime();
    }
/*** if VER == 2 */
    if (t > start + run_t) {
      sleep_sec(slp_t);
      start = cur_time();
    }
/*** endif */
  }
  assert(i < n);
  int j;
  for (j = 0; j <= i; j++) {
    printf("%d %d %f %f %f %f\n", 
	   pid, R[j].proc,
           R[j].begin, R[j].end, R[j].end - R[j].begin,
           R[j].vruntime);
  }
  return 0;
}

int main(int argc, char ** argv) {
  long i = 1;
  double T     = (argc > i ? atof(argv[i]) : 10.0);    i++; /* 合計時間 */
/*** if VER == 2 */
  double run_t = (argc > i ? atof(argv[i]) : T);       i++; /* 1回走り続ける時間 */
  double slp_t = (argc > i ? atof(argv[i]) : 0);       i++; /* 1回sleepする時間 */
/*** endif */
  long n       = (argc > i ? atoi(argv[i]) : 1000000); i++;
/*** if VER == 1 */
  run(T, n);
/*** elif VER == 2 */
  run(T, run_t, slp_t, n);
/*** else */
  (void)T;
  (void)n;
/*** endif */
  return 0;
}
