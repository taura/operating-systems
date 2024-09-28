#com 2
#include <assert.h>
#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

/* 1イベントの記録 */
typedef struct {
  struct timeval tv;
  struct rusage ru;
  size_t offset;                /* 読んだ場所 */
} record_t;

/* 全イベントの記録 */
typedef struct {
  long n_records;              /* 配列aのサイズ */
  long i;                       /* 次に書き込むインデクス */
  record_t * a;                 /* recordの配列 */
} records_t;

const long page_sz = 4096;

/* n_records分の記録を保持できるrecords_tを作る */
records_t * mk_records(long n_records) {
  record_t * a = (record_t *)malloc(sizeof(record_t) * n_records);
  memset(a, 1, sizeof(record_t) * n_records);
  /* records_tの割当て */
  records_t * R = (records_t *)malloc(sizeof(records_t));
  R->n_records = n_records;
  R->i = 0;
  R->a = a;
  return R;
}

void destroy_records(records_t * R) {
  free(R);
}

/* 1イベントの記録 */
void record_access(records_t * R) {
  long i = R->i;
  record_t * r = &R->a[i];
  if (gettimeofday(&r->tv, 0) == -1) { err(1, "gettimeofday"); }
  if (getrusage(RUSAGE_SELF, &r->ru) == -1) { err(1, "getrusage"); }
  R->i = i + 1;
}

/* 全イベントの記録を標準出力へ表示 */
void print_records(records_t * R) {
  long m = R->i;
  for (long i = 0; i < m; i++) {
    record_t * r0 = &R->a[0];
    record_t *  r = &R->a[i];
    double t0 = r0->tv.tv_sec + r0->tv.tv_usec * 1.0e-6;
    double t  =  r->tv.tv_sec +  r->tv.tv_usec * 1.0e-6;
    printf("%ld %f %ld %ld %ld %ld\n",
           i,
           t - t0,
           r->ru.ru_minflt,
           r->ru.ru_majflt,
           r->ru.ru_inblock,
           r->ru.ru_oublock);
  }
}

/* szバイトぴったり読む. その前にEOFになったらエラー */
void read_sz(int fd, char * buf, size_t sz) {
  size_t rd = 0;
  while (rd < sz) {
    ssize_t x = read(fd, buf + rd, sz - rd);
    if (x == -1) err(1, "read");
    assert(x > 0);
    rd += x;
  }
  assert(rd == sz);
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
 */
int main(int argc, char ** argv) {
  int i = 1;
  char * const filename = (argc > i ? argv[i] : "data.bin"); i++;
  /* 使うデータサイズ (ファイルの先頭から; MB単位) */
  const long data_sz   = (argc > i ? parse_size(argv[i]) : parse_size("64m")); i++;
  /* ファイル全体を読む周回数 */
  const long n_times    = (argc > i ? atol(argv[i]) : 3); i++;
  /* 1周でアクセスする量 */
  const long stride = page_sz;
  const long n_accesses = data_sz / stride;
  const long n_records = n_accesses * n_times + 1;
  records_t * const R = mk_records(n_records);

  fprintf(stderr, "read %ld bytes %ld times\n",
          data_sz, n_times); fflush(stderr);
  for (long i = 0; i < n_times; i++) {
    fprintf(stderr, "%ld th read starts\n", i); fflush(stderr);
    record_access(R);
    const int fd = open(filename, O_RDONLY);
    if (fd == -1) err(1, "open");
#ifpy VER == 1
    char * const a = mmap(0, data_sz, PROT_READ, MAP_PRIVATE, fd, 0);
    if (a == MAP_FAILED) err(1, "mmap");
#elifpy VER == 2
    char * const a = (char *)malloc(data_sz);
    if (!a) err(1, "malloc");
    read_sz(fd, a, data_sz);
#endifpy
    long s = 0;
    for (long j = 0; j < n_accesses; j++) {
      s += a[j * stride];
      record_access(R);
    }
#ifpy VER == 1
    munmap(a, data_sz);
#elifpy VER == 2
    free(a);
#endifpy
    if (close(fd) == -1) err(1, "close");
    fprintf(stderr, "sum = %ld\n", s);
  }
  print_records(R);
  destroy_records(R);
  return 0;
}
