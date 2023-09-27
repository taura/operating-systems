/* 
 * openmp_visualize_lock.c
 * compile this file by:
      gcc openmp_visualize_lock.c -lpthread
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <omp.h>
#include <pthread.h>
#include <sys/time.h>

double cur_time() {
  struct timespec ts[1];
  clock_gettime(CLOCK_REALTIME, ts);
  return ts->tv_sec + ts->tv_nsec * 1.0E-9;
}

/* s 秒時間を使う(計算の振り) */
void think(double s) {
  double t0 = cur_time();
  while (cur_time() - t0 < s) { }
}

/* ロックの記録 */
typedef struct {
  double enter_lock_time;	/* lockを呼び出した時刻 */
  double return_from_lock_time;	/* lockが取得された(lockからreturnした)時刻 */
  double enter_unlock_time;	/* unlockを呼び出した時刻 */
  long x;			/* sequence番号 */
} record;

/* ロックの記録を格納する配列 */
typedef struct {
  record * a;		 /* recordの配列(n要素) */
  long i;		 /* 次の要素の番号 */
  long n;		 /* aの容量 */
} record_buf;

/* n要素の記録用配列を作る */
record_buf * make_record_buf(long n) {
  record * a = (record *)calloc(sizeof(record), n);
  record_buf * R = (record_buf *)malloc(sizeof(record_buf));
  R->a = a;
  R->n = n;
  R->i = 0;
  return R;
}

/* lockを呼び出すときに呼び出す */
void enter_lock(record_buf * R) {
  long i = R->i;
  assert(i < R->n);
  R->a[i].enter_lock_time = cur_time();
}

/* lockからreturnした時に呼び出す */
void return_from_lock(record_buf * R) {
  long i = R->i;
  assert(i < R->n);
  R->a[i].return_from_lock_time = cur_time();
}

/* unlockを呼び出すときに呼び出す */
void enter_unlock(record_buf * R, long x) {
  long i = R->i;
  assert(i < R->n);
  R->a[i].enter_unlock_time = cur_time();
  R->a[i].x = x;
  R->i = i + 1;
}

/* lockを呼び出す; 前後で記録を取る */
int lock_vis(pthread_mutex_t * l, 
	     record_buf * R) {
  enter_lock(R);
  int r = pthread_mutex_lock(l);
  return_from_lock(R);
  return r;
}

/* unlockを呼び出す; 前で記録を取る */
int unlock_vis(pthread_mutex_t * l, 
	       record_buf * R, long x) {
  enter_unlock(R, x);
  return pthread_mutex_unlock(l);
}

/* 記録をファイルに保存 */
pthread_mutex_t file_mutex[1];
void dump_record_buf(record_buf * R, int idx, FILE * wp) {
  long n = R->n;
  long i;
  pthread_mutex_lock(file_mutex);
  for (i = 0; i < n; i++) {
    fprintf(wp, "%ld %d enter_lock %.9f return_from_lock %.9f enter_unlock %.9f\n",
	    R->a[i].x, idx,
	    R->a[i].enter_lock_time, R->a[i].return_from_lock_time,
	    R->a[i].enter_unlock_time);
  }
  pthread_mutex_unlock(file_mutex);
}


long g = 0;
void * thread_func(long n_inc, pthread_mutex_t * m, FILE * wp) {
  int idx = omp_get_thread_num();
  record_buf * R = make_record_buf(n_inc);
  long i;
  
  for (i = 0; i < n_inc; i++) {
    lock_vis(m, R);	       /* lock取得 */
    long x = g++;
    think(1.0e-4);		/* 0.1 ms 計算 */
    unlock_vis(m, R, x);
    think(1.0e-4);		/* 0.1 ms 計算 */
  }
  dump_record_buf(R, idx, wp);
  
  printf("g = %ld\n", g);
  return 0;
}

int main(int argc, char ** argv)
{
  if (argc <= 2) {
    fprintf(stderr, 
	    "usage: OMP_NUM_THREADS=no_of_threads %s no_of_increments log_file\n"
	    "example:\n   OMP_NUM_THREADS=16 %s 10000000 log.txt\n",
	    argv[0], argv[0]);
    exit(1);
  }
  long n_inc = atol(argv[1]);
  char * log_file = argv[2];

  double t0 = cur_time();
  pthread_mutex_t m[1];

  pthread_mutex_init(m, 0);
  pthread_mutex_init(file_mutex, 0);

  FILE * wp = fopen(log_file, "wb");
  if (!wp) { perror("fopen"); exit(1); }

  /* スレッドを N_THREADS 個作る */
#pragma omp parallel 
  {
    thread_func(n_inc, m, wp);
  }
  double t1 = cur_time();
  printf("OK: elapsed time: %f\n", t1 - t0);

  fclose(wp);
  return 0;
}

