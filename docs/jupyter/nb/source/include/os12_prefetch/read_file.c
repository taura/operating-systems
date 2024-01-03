#com 5
#define _GNU_SOURCE             /* See feature_test_macros(7) */
#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#ifpy VER >= 5
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#endifpy

/* イベントの種類 */
typedef enum {
  ek_read_enter,
  ek_read_return,
#ifpy VER >= 3
  ek_read_ahead_enter,
  ek_read_ahead_return,
#endifpy
} event_kind_t;

/* 1イベントの記録 */
typedef struct {
  double t;                     /* 時刻 */
  event_kind_t kind;            /* イベントの種類(read呼び出し, read復帰, etc.) */
  size_t offset;                /* 読んだ場所 */
  size_t size;                  /* 読んだサイズ */
} record_t;

/* 全イベントの記録 */
typedef struct {
  long n_records;              /* 配列aのサイズ */
  long i;                       /* 次に書き込むインデクス */
  record_t * a;                 /* recordの配列 */
  int fd;                       /* ファイルディスクリプタ */
  long data_sz;                 /* ファイル(使う部分)のサイズ */
#ifpy VER >= 5
  char * mapped;                /* ファイルをmmapした領域(mincore用) */
  unsigned char * incore;       /* mincoreの結果を得る配列 */
#endifpy
} records_t;

const char * event_kind_string(event_kind_t k) {
  switch (k) {
  case ek_read_enter:
    return "read_enter";
  case ek_read_return:
    return "read_return";
#ifpy VER >= 3
  case ek_read_ahead_enter:
    return "read_ahead_enter";
  case ek_read_ahead_return:
    return "read_ahead_return";
#endifpy
  default:
    assert(0);
  }
}

const long page_sz = 4096;

/* n_records分の記録を保持できるrecords_tを作る */
records_t * mk_records(long n_records
#ifpy VER >= 5
                       , char * filename, long data_sz
#endifpy
                       ) {
  record_t * a = (record_t *)malloc(sizeof(record_t) * n_records);
  memset(a, 0, sizeof(record_t) * n_records);
#ifpy VER >= 5  
  /* 全体をカバーするページ数 */
  long n_pages = (data_sz + page_sz - 1) / page_sz;
  /* mincore用の領域 */
  unsigned char * incore = malloc(n_pages);
  if (!incore) err(1, "malloc");
  /* mincore用にファイルをmmap */
  int fd = open(filename, O_RDONLY);
  if (fd == -1) err(1, "open");
  char * mapped = mmap(0, data_sz, PROT_READ, MAP_SHARED, fd, 0);
  if (mapped == MAP_FAILED) err(1, "mmap");
#endifpy
  /* records_tの割当て */
  records_t * R = (records_t *)malloc(sizeof(records_t));
  R->n_records = n_records;
  R->i = 0;
  R->a = a;
#ifpy VER >= 5  
  R->fd = fd;
  R->mapped = mapped;
  R->data_sz = data_sz;
  R->incore = incore;
#endifpy
  return R;
}

void destroy_records(records_t * R) {
#ifpy VER >= 5  
  if (munmap(R->mapped, R->data_sz) == -1) err(1, "munmap");
  if (close(R->fd) == -1) err(1, "close");
  free(R->incore);
  free(R->a);
#endifpy
  free((records_t *)R);
}

/* 時刻を返す */
double cur_time() {
  struct timeval tp[1];
  gettimeofday(tp, 0);
  double t = tp->tv_sec + 1.0e-6 * tp->tv_usec;
  return t;
}
#ifpy VER >= 5

/* キャッシュされている(物理メモリ上にある)バイト数を返す */
long count_incore(records_t * R) {
  if (mincore(R->mapped, R->data_sz, R->incore) == -1) {
    err(1, "mincore");
  }
  long bytes_incore = 0;
  for (long i = 0; i * page_sz < R->data_sz; i++) {
    if (R->incore[i]) bytes_incore += page_sz;
  }
  return bytes_incore;
}
#endifpy

/* 1イベントの記録 */
void record_event(records_t * R, event_kind_t k, size_t offset, size_t size) {
  long i = __sync_fetch_and_add(&R->i, 1);
  assert(i < R->n_records);
  R->a[i].t = cur_time();
  R->a[i].kind = k;
  R->a[i].offset = offset;
  R->a[i].size = size;
}

/* 全イベントの記録を標準出力へ表示 */
void print_records(records_t * R) {
  long m = R->i;
  for (long i = 0; i < m; i++) {
    printf("%f %s %ld %ld\n",
           R->a[i].t - R->a[0].t,
           event_kind_string(R->a[i].kind),
           R->a[i].offset,
           R->a[i].size);
  }
}
#ifpy VER >= 2

/* 長さ n_blocks の配列 a を返す. 
   a[i] は i 番目に読むべきファイルの位置.
   seed == 0 の場合 -> 逐次 
   seed != 0 の場合 -> ランダム */
ssize_t * make_offsets(int block_sz, int n_blocks, long seed) {
  ssize_t * a = (ssize_t *)malloc(sizeof(ssize_t) * n_blocks);
  int i = 0;
  for (i = 0; i < n_blocks; i++) {
    a[i] = i * block_sz;
  }
  if (seed) {
    unsigned short rg[3] = {(unsigned short)(seed >> 16),
                            (unsigned short)(seed >> 8),
                            (unsigned short)(seed >> 0)};
    /* 配列のかき混ぜ (10 * 要素数回くらい, 適当な2要素を入れ替え) */
    for (i = 0; i < 10 * n_blocks; i++) {
      int j = nrand48(rg) % n_blocks;
      int k = nrand48(rg) % n_blocks;
      int t = a[j];
      a[j] = a[k];
      a[k] = t;
    }
  }
  return a;
}
#endifpy
#ifpy VER >= 3

/* ファイルfdの [offset, offset+sz) の範囲をreadahead */
void read_ahead(int fd, off64_t offset, size_t sz, records_t * R) {
  record_event(R, ek_read_ahead_enter, offset, sz);
  ssize_t r = readahead(fd, offset, sz);
  if (r == -1) err(1, "readahead");
  record_event(R, ek_read_ahead_return, offset, sz);
}
#endifpy

/* szバイトぴったり読む. その前にEOFになったらエラー */
long read_sz_at(int fd, char * buf, off_t offset, size_t sz, records_t * R) {
  //if (lseek(fd, offset, SEEK_SET) == -1) err(1, "lseek");
  record_event(R, ek_read_enter, offset, sz);
  size_t rd = 0;
  while (rd < sz) {
    ssize_t x = pread(fd, buf + rd, sz - rd, offset + rd);
    if (x == -1) err(1, "read");
    assert(x > 0);
    rd += x;
  }
  assert(rd == sz);
  size_t begin_offset = offset + page_sz - 1;
  begin_offset -= begin_offset % page_sz;
  long s = 0;
  for (size_t o = begin_offset; o < offset + sz; o += page_sz) {
    s += buf[o - offset];
  }
  record_event(R, ek_read_return, offset, sz);
  return s;
}

/* 10m, 100k みたいな文字列を読んで数に変換
   e.g., 10k -> 10 * 1024 */
long parse_size(char * s) {
  long n = strlen(s);
  long unit = -1;
  assert(n > 0);
  switch (s[n - 1]) {
  case 'K' :
  case 'k' :
    unit = 1024L;
    break;
  case 'M' :
  case 'm' :
    unit = 1024L * 1024L;
    break;
  case '0' ... '9' :
    unit = 1L;
    break;
  case 'G' :
  case 'g' :
    fprintf(stderr, "do you mean GB? don't be so aggressive\n");
    break;
  case 'T' :
  case 't' :
    fprintf(stderr, "do you mean TB? don't be so aggressive\n");
    break;
  default:
    fprintf(stderr, "invalid unit (%c) specified\n", s[n - 1]);
    break;
  }
  if (unit == -1) return -1;
  long x = atol(s);
  assert(x > 0);
  return x * unit;
}

/* 
#ifpy VER == 1
./%%BASENAME%%_%%VER%% FILE SZ N_TIMES BLK_SZ
#elifpy VER == 2
./%%BASENAME%%_%%VER%% FILE SZ N_TIMES BLK_SZ SEED
#elifpy VER == 3
./%%BASENAME%%_%%VER%% FILE SZ N_TIMES BLK_SZ SEED N_READAHEADS
#elifpy VER == 4
OMP_NUM_THREADS=N ./%%BASENAME%%_%%VER%% FILE SZ N_TIMES BLK_SZ SEED
#elifpy VER == 5
OMP_NUM_THREADS=N ./%%BASENAME%%_%%VER%% FILE SZ N_TIMES BLK_SZ SEED N_READAHEADS
#endifpy
 */
int main(int argc, char ** argv) {
  int i = 1;
  char * const filename = (argc > i ? argv[i] : "data.bin"); i++;
  /* 使うデータサイズ (ファイルの先頭から; MB単位) */
  const long data_sz_   = (argc > i ? parse_size(argv[i]) : parse_size("64m")); i++;
  /* ファイル全体を読む周回数 */
  const long n_times    = (argc > i ? atol(argv[i]) : 3); i++;
  /* read一回で読む量 */
  const long block_sz   = (argc > i ? parse_size(argv[i]) : parse_size("4k")); i++;
#ifpy VER >= 2
  /* 乱数 seed (0の場合は逐次) */
  const long seed       = (argc > i ? atol(argv[i]) : 0); i++;
#endifpy
#ifpy VER >= 3
  /* 先読みする数 */
  const long n_aheads   = (argc > i ? atol(argv[i]) : 0); i++;
#endifpy
  /* ブロック数 (中途半端は切り捨て) */
  const long n_blocks   = data_sz_ / block_sz;
  const long data_sz    = block_sz * n_blocks;
  /* 読み出すオフセットを作成 */
#ifpy VER >= 3
  const long n_records  = 4 * n_blocks * n_times;
#elsepy
  const long n_records  = 2 * n_blocks * n_times;
#endifpy  
#ifpy VER >= 2
  ssize_t * const offsets = make_offsets(block_sz, n_blocks, seed);
#endifpy
#ifpy VER >= 5
  records_t * const R = mk_records(n_records, filename, data_sz);
#elsepy
  records_t * const R = mk_records(n_records);
#endifpy

  fprintf(stderr, "read %ld bytes %ld times, %ld bytes at a time\n",
          data_sz, n_times, block_sz); fflush(stderr);
  for (long i = 0; i < n_times; i++) {
    fprintf(stderr, "%ld th read starts\n", i); fflush(stderr);
    const double t0 = cur_time();
    const int fd = open(filename, O_RDONLY);
    if (fd == -1) err(1, "open");
    long s = 0;
#ifpy VER >= 4
#pragma omp parallel reduction(+:s)
#endifpy
    {
      char * const buf = (char *)malloc(block_sz);
      if (!buf) err(1, "malloc");
#ifpy VER >= 4
#pragma omp for schedule(dynamic)
#endifpy
#ifpy VER == 1
      for (long j = 0; j < n_blocks; j++) {
        s += read_sz_at(fd, buf, j * block_sz, block_sz, R);
      }
#elifpy VER == 2
      for (long j = 0; j < n_blocks; j++) {
        if (j >= 0) {
          s += read_sz_at(fd, buf, offsets[j], block_sz, R);
        }
      }
#elifpy VER >= 3
      /* n_aheads回分のreadahead */
      for (long j = -n_aheads; j < n_blocks; j++) {
        if (j >= 0) {
          s += read_sz_at(fd, buf, offsets[j], block_sz, R);
        }
        if (n_aheads > 0 && j + n_aheads < n_blocks) {
          read_ahead(fd, offsets[j + n_aheads], block_sz, R);
        }
      }
#endifpy
      free(buf);
    }
    if (close(fd) == -1) err(1, "close");
    double t1 = cur_time();
    double dt = t1 - t0;
    fprintf(stderr, "took %f sec, %f MB/sec, sum = %ld\n",
            dt, data_sz / (1024.0 * 1024.0) / dt, s);
    fflush(stderr);
  }
  print_records(R);
  destroy_records(R);
  return 0;
}
