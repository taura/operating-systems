#com 5
//% file: %%BASENAME%%_%%VER%%.c
//% cmd: gcc -O3 -Wall -Wextra -o %%BASENAME%%_%%VER%% %%BASENAME%%_%%VER%%.c

#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

const long page_sz = 4096;

/* 実行時間とページフォルト回数の記録 */
typedef struct {
  struct timeval tv;
  struct rusage ru;
} timeval_rusage;

/* 実行時間とページフォルト回数をtrに記録する */
void record_timeval_rusage(timeval_rusage * tr) {
  if (gettimeofday(&tr->tv, 0) == -1) { err(1, "gettimeofday"); }
  if (getrusage(RUSAGE_SELF, &tr->ru) == -1) { err(1, "getrusage"); }
}

/* 実行時間とページフォルト回数の記録tr[0]〜tr[n_records-1]
   をファイルtime_usage_datに保存する */
void save_timeval_rusage(const char * time_usage_dat,
                         timeval_rusage * tr,
                         long n_records,
                         long record_interval) {
  FILE * wp = fopen(time_usage_dat, "w");
  if (!wp) { err(1, "fopen"); }
  for (long i = 0; i < n_records; i++) {
    fprintf(wp, "%ld\t%f\t%ld\t%ld\n", 
            i * record_interval * 4096,
            tr[i].tv.tv_sec + tr[i].tv.tv_usec * 1.0e-6,
            tr[i].ru.ru_minflt,
            tr[i].ru.ru_majflt);
  }
  fclose(wp);
}

#ifpy VER >= 2
/* addr[0:length]中のどのページがメモリ上にあるか
   + 最後にアクセスしたアドレス(last_accessed)を記録する */
void record_incore(FILE * wp, 
		   void * addr, size_t length, unsigned char * R,
		   long * mru, long n_mru) {
  size_t n_pages = (length + page_sz - 1) / page_sz;
  struct timeval tp[1];
  if (gettimeofday(tp, NULL) == -1) { perror("gettimeofday"); exit(1); }
  double t = tp->tv_sec + tp->tv_usec * 1.0e-6;
  /* get current time */
  fprintf(wp, "%f\n", t);
  if (mincore(addr, length, R) == -1) {
    err(1, "mincore");
  }
  fprintf(wp, "%lu\n", n_pages);
  size_t c = fwrite(R, n_pages, 1, wp);
  if (c != 1) {
    err(1, "fwrite");
  }

  fprintf(wp, "%ld\n", n_mru);
  c = fwrite(mru, sizeof(long), n_mru, wp);
  if (c != (size_t)n_mru) {
    err(1, "fwrite");
  }
}
#endifpy

#ifpy VER == 4
typedef struct {
  long ts;                      /* ts >= 0 <--> on memory */
} timestamp_t;

/* 配列の後半(target_resident_pages ページ目以降)を evict する */
long evict_pages(char * a, long sz, 
                 timestamp_t * timestamps,
                 long target_resident_pages,
                 long n_resident_pages) {
  (void)n_resident_pages;
  long n_pages = (sz + page_sz - 1) / page_sz;
  for (long i = n_pages - target_resident_pages; i < n_pages; i++) {
    timestamps[i].ts = -1;
  }
  long offset = target_resident_pages * page_sz;
  long len = (n_pages - target_resident_pages) * page_sz;
  if (madvise(a + offset, len, MADV_PAGEOUT) == -1) {
    err(1, "madvise");
  }
  return target_resident_pages;
}
#endifpy

#ifpy VER == 5
void evict(char * a, long start, long end) {
  printf(" evict %ld:%ld\n", start, end);
  long offset = start * page_sz;
  long len = (end - start) * page_sz;
  if (madvise(a + offset, len, MADV_PAGEOUT) == -1) {
    err(1, "madvise");
  }
}

typedef struct {
  long page;
  long ts;
} timestamp_t;

int cmp_by_timestamp(const void * a_, const void * b_) {
  const timestamp_t * a = a_;
  const timestamp_t * b = b_;
  return a->ts - b->ts;
}

int cmp_by_page(const void * a_, const void * b_) {
  const timestamp_t * a = a_;
  const timestamp_t * b = b_;
  return a->page - b->page;
}

long evict_pages(char * a, long sz, 
                 timestamp_t * timestamps,
                 long target_resident_pages,
                 long n_resident_pages) {
  printf("evict_pages %ld -> %ld\n", n_resident_pages, target_resident_pages);
  long n_pages = (sz + page_sz - 1) / page_sz;
  qsort(timestamps, n_pages, sizeof(timestamp_t), cmp_by_timestamp);
  long thrown_pages = n_resident_pages - target_resident_pages;
  long nr = 0;
  for (long i = 0; i < n_pages; i++) {
    if (timestamps[i].ts != -1) {
      nr++;
    }
  }
  assert(nr == n_resident_pages);
  for (long i = n_pages - thrown_pages; i < n_pages; i++) {
    assert(timestamps[i].ts != -1);
    timestamps[i].ts = -1;
  }
  nr = 0;
  for (long i = 0; i < n_pages; i++) {
    if (timestamps[i].ts != -1) {
      nr++;
    }
  }
  assert(nr == n_resident_pages - thrown_pages);
  assert(nr == target_resident_pages);
  qsort(timestamps, n_pages, sizeof(timestamp_t), cmp_by_page);
  long evict_start = 0;
  long n_residents_after = 0;
  for (long i = 0; i < n_pages; i++) {
    if (timestamps[i].ts != -1) {
      n_residents_after++;
      if (evict_start < i) {
        evict(a, evict_start, i);
      }
      evict_start = i + 1;
    }
  }
  if (evict_start < n_pages) {
    evict(a, evict_start, n_pages);
  }
  assert(n_residents_after == target_resident_pages);
  return n_residents_after;
}
#endifpy

/* 
   usage:
     ./page_fault_rec N S A RI
   (1) N MBの領域を割り当てる (デフォルト64 MB)
   (2) S > 0ならば二つのアクセス間の距離 (デフォルト 4096) -1ならばランダム
   (3) A 回 (2)に従ってページをアクセス (デフォルト -1). -1の場合, A = N * 256 * 3
       (N MB のページ数 x 3)
   (4) RIアクセスごとに時刻と, ページフォルト数を記録する 
       (デフォルト 500アクセス)
#ifpy VER >= 2
   (5) RIアクセスごとに, メモリ上にあるページを記録する
#endifpy
 */

int main(int argc, char ** argv) {
  int i = 1;
  /* 配列のサイズ(MB) */
  long n_mb            = (argc > i ? atol(argv[i]) : 64); i++;
#ifpy VER != 3
  /* STRIDE>0 -> バイト飛ばしでアクセス */
  long stride          = (argc > i ? atol(argv[i]) : page_sz); i++;
#endifpy
  /* アクセス回数(デフォルトは配列のページ数 x 3回) */
  long n_accesses      = (argc > i ? atol(argv[i]) : -1); i++;
  /* 記録間隔 */
  long record_interval = (argc > i ? atol(argv[i]) : 500); i++;
#ifpy VER == 3
  /* 乱数の種 */
  long seed            = (argc > i ? atol(argv[i]) : 1234567890123L); i++;
#endifpy
#ifpy VER >= 4
  long max_resident_mb    = (argc > i ? atol(argv[i]) : 224); i++;
  long target_resident_mb = (argc > i ? atol(argv[i]) : 192); i++;
#endifpy
  long sz = n_mb * 1024L * 1024L; /* size in bytes */
  long n_pages = (sz + page_sz - 1) / page_sz;
  if (n_accesses == -1) n_accesses = n_pages * 3;
  long n_records = (n_accesses + record_interval - 1) / record_interval;
  char * a = (char *)sbrk(sz);
  if (!a) { err(1, "sbrk"); }
  timeval_rusage * tr = malloc(sizeof(timeval_rusage) * n_records);
  if (!tr) { err(1, "malloc"); }
#ifpy VER >= 2
  unsigned char * R = calloc(sizeof(unsigned char), n_pages);
  if (!R) { err(1, "calloc"); }
  FILE * wp = fopen("mincore.dat", "wb");
  if (!wp) { err(1, "fopen"); }
  long * mru = calloc(sizeof(long), record_interval);
  if (!mru) { err(1, "calloc"); }
#endifpy
#ifpy VER == 3
  unsigned short rg[3] = { seed >> 32, seed >> 16, seed };
#endifpy
#ifpy VER >= 4
  long max_resident_pages = max_resident_mb * 1024L * 1024L / page_sz;
  long target_resident_pages = target_resident_mb * 1024L * 1024L / page_sz;
  timestamp_t * timestamps = malloc(sizeof(timestamp_t) * n_pages);
  for (long i = 0; i < n_pages; i++) {
#ifpy VER >= 5
    timestamps[i].page = i;
#endifpy
    timestamps[i].ts = -1;
  }
#endifpy
  long idx = 0;                 /* 次にアクセスする要素 */
  /* report_interval 回アクセスごとに . を打つ */
  const long n_dots = 100;
  long report_interval = (n_accesses + n_dots - 1) / n_dots;

#ifpy VER >= 4
  long n_resident_pages = 0;
#endifpy
  fprintf(stderr, "touching %ld MB %ld times\n", n_mb, n_accesses);
  long s = 0;
  for (long i = 0; i < n_accesses; i++) {
    if (i % report_interval == 0) {
      /* 進捗表示 */
      putchar('.'); fflush(stdout);
    }
    if (i % record_interval == 0) {
      /* 時間を記録 */
      long k = i / record_interval; /* 記録回数 */
      assert(k < n_records);
      record_timeval_rusage(&tr[k]);
#ifpy VER >= 2
      /* メモリ上のページを記録 */
      record_incore(wp, a, sz, R, mru, (i ? record_interval : 0));
#endifpy
    }
    idx = idx % sz;
    s += (a[idx]++);
#ifpy VER >= 2
    mru[i % record_interval] = idx;
#endifpy
#ifpy VER >= 4
    long page = idx / page_sz;
    if (timestamps[page].ts == -1) {
      n_resident_pages++;
    }
    timestamps[page].ts = i;
    /* メモリ上のページ数がmax_resident_pagesになったら,
       はじめの target_resident_pages ページを除いて,
       madvise(... PAGE_OUT) */
    if (n_resident_pages == max_resident_pages) {
      n_resident_pages
        = evict_pages(a, sz, timestamps,
                      target_resident_pages,
                      max_resident_pages);
      assert(n_resident_pages == target_resident_pages);
    }
#endifpy
#ifpy VER == 3
    idx = nrand48(rg);
#elsepy
    idx += stride;
#endifpy
  }
  assert((n_accesses - 1) / record_interval + 1 == n_records);
#ifpy VER >= 2
  fclose(wp);
#endifpy
  /* save time and page faults */
  save_timeval_rusage("time_rusage.dat", tr, n_records, record_interval);
  printf("\ndone s = %ld\n", s);
  return 0;
}

