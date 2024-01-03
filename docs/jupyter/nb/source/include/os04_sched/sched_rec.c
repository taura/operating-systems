#com 5
#define _GNU_SOURCE
#include <assert.h>
#include <sched.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifpy VER == 3
#include <sys/types.h>
#include <sys/wait.h>
#elifpy VER == 5
#include <pthread.h>
#endifpy

/* 時刻 begin -- end まで proc 上で動いていた記録 */
typedef struct {
  double begin;
  double end;
  int proc;
} rec_t;

/* 現在時刻を得る */
double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, 0);
  return tp->tv_sec + tp->tv_usec * 1.0E-6;
}

/* T秒間走り続け, CPUが割り当てられていたと思しき時間帯を記録する */
int run(double T, int n) {
#ifpy VER < 5
  pid_t pid = getpid();
#elsepy
  pid_t pid = gettid();
#endifpy
  double limit = cur_time() + T;
  rec_t * R = (rec_t *)calloc(n, sizeof(rec_t));
  int i = 0;
  R[i].begin = R[i].end = cur_time();
  R[i].proc = sched_getcpu();

  while (R[i].end < limit && i < n) {
    double t = cur_time(); /* 現在時刻を得る */
    int proc = sched_getcpu();
    if (t - R[i].end < 1.0E-3 && proc == R[i].proc) {
      /* 最後に見た時刻とあまり変わらない(< 1ms) -> R[i].endを増やす */
      R[i].end = t;
    } else {
      /* 最後に見た時刻から1ms以上たっている -> 新しい区間に入る */
      if (i + 1 >= n) break;
      i++;
      R[i].proc = proc;
      R[i].begin = R[i].end = cur_time();
    }
  }
  assert(i < n);
  int j;
  for (j = 0; j <= i; j++) {
    printf("%d %f %f %d %f\n", 
           pid, R[j].begin, R[j].end, R[j].proc,
           R[j].end - R[j].begin);
  }
  return 0;
}

#ifpy VER == 5                  # thread
typedef struct {
  pthread_t thread_id;
  double T;
  int n;
} thread_args_t;

void * thread_fun(void * args_) {
  thread_args_t * args = args_;
  run(args->T, args->n);
  return 0;
}

#endifpy

int main(int argc, char ** argv) {
  double T = (argc > 1 ? atof(argv[1]) : 10.0);
  int n    = (argc > 2 ? atoi(argv[2]) : 100000);
#ifpy VER == 3 or VER == 5
  int m    = (argc > 3 ? atoi(argv[3]) : 4);
#endifpy

#ifpy VER == 1 or VER == 2 or VER == 4
  run(T, n);
#elifpy VER == 3                  # fork
  pid_t pids[m];
  for (int i = 0; i < m; i++) {
    pid_t pid = fork();
    if (pid == -1) err(1, "fork");
    else if (pid == 0) {
      /* child */
      run(T, n);
      exit(0);
    }
    pids[i] = pid;
  }
  for (int i = 0; i < m; i++) {
    int ws;
    pid_t pid = waitpid(pids[i], &ws, 0);
    if (pid == -1) err(1, "waitpid");
  }
#elifpy VER == 5                  # thread
  thread_args_t args[m];
  for (int i = 0; i < m; i++) {
    args[i].T = T;
    args[i].n = n;
    if (pthread_create(&args[i].thread_id, 0, thread_fun, &args[i])) {
      err(1, "pthread_create");
    }
  }
  for (int i = 0; i < m; i++) {
    if (pthread_join(args[i].thread_id, 0)) {
      err(1, "pthread_join");
    }
  }
#elsepy
  (void)T;
  (void)n;
#endifpy
  return 0;
}
