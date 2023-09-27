#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

/* /proc/self/sched を読んで，
   自分のvruntime をdouble で返す */
double read_sched(int n) {
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
  fseek(fp, 68 * n, SEEK_CUR);
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

double cur_time() {
#if 1
  struct timespec tp[1];
  clock_gettime(CLOCK_REALTIME, tp);
  return tp->tv_sec + tp->tv_nsec * 1.0e-9;
#else
  struct timeval tp[1];
  gettimeofday(tp, NULL);
  return tp->tv_sec + tp->tv_usec * 1.0e-6;
#endif
}

typedef struct {
  double t;
  double vruntime;
} record;

#define VRUNTIME_LINE_NO 1

/* slp 秒だけ休む */
void dsleep(double slp) {
  struct timespec tp[1];
  struct timespec tr[1];
  int r;
  tp->tv_sec = (long)slp;
  tp->tv_nsec = (slp - (long)slp) * 1.0e9;
  r = nanosleep(tp, tr);
  if (r) {
    perror("nanosleep");
    exit(1);
  }
  //assert(tr->tv_sec == 0);
  //assert(tr->tv_nsec == 0);
}

/* run秒走って，slp秒寝るを繰り返す．
   gran秒おきに一回vruntimeを記録． */
record * plot_time_and_vruntime_with_sleep(double gran, long n, double run, double slp) {
  double t0 = cur_time();
  double last_record_t = t0;
  double last_wake_t = t0;
  long i = 0;
  record * R = (record *)calloc(sizeof(record), n);
  while (i < n) {
    double t = cur_time();
    if (t > last_record_t + gran) {
      R[i].t = t;
      R[i].vruntime = read_sched(VRUNTIME_LINE_NO) * 1.0e-3;
      i++;
      last_record_t = t;
    }
    if (slp > 0.0 && t > last_wake_t + run) {
      dsleep(slp);
      last_wake_t = cur_time();
    }
  }
  return R;
}

/* ひたすら走る．gran秒に一度，vruntimeを記録 */
record * plot_time_and_vruntime(double gran, long n) {
  double t0 = cur_time();
  double last_record_t = t0;
  long i = 0;
  record * R = (record *)calloc(sizeof(record), n);
  while (i < n) {
    double t = cur_time();
    if (t > last_record_t + gran) {
      R[i].t = t;
      R[i].vruntime = read_sched(VRUNTIME_LINE_NO) * 1.0e-3;
      i++;
      last_record_t = t;
    }
  }
  return R;
}

/* 時刻 vruntime をずらずら表示 */
void show_record(record * R, long n) {
  long i;
  for (i = 0; i < n; i++) {
    printf("%.9f %.9f\n", R[i].t, R[i].vruntime);
  }
}

int main(int argc, char ** argv) {
  double gran = (argc > 1 ? atof(argv[1]) : 1.0e-4);
  long n      = (argc > 2 ? atol(argv[2]) : 10000);
  double run  = (argc > 3 ? atof(argv[3]) : 1.0);
  double slp  = (argc > 4 ? atof(argv[4]) : 0.0);
  record * R = plot_time_and_vruntime_with_sleep(gran, n, run, slp);
  show_record(R, n);
  return 0;
}
