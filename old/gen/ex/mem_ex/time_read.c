#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

typedef struct block_record
{
  double t;
  ssize_t bytes_read;
} block_record, * block_record_t;

struct timeval start_time[1];

int set_start_time()
{
  return gettimeofday(start_time, 0);
}

double cur_time()
{
  struct timeval tp[1];
  gettimeofday(tp, 0);
  return (tp->tv_sec - start_time->tv_sec) +
    (tp->tv_usec - start_time->tv_usec) * 0.000001;
}

off_t file_size(char * filename)
{
  struct stat sbuf[1];
  if (-1 == stat(filename, sbuf)) {
    perror("stat");
    exit(1);
  } else {
    return sbuf->st_size;
  }
}

ssize_t * make_pointers(int block_sz, int nblocks, char * mode)
{
  ssize_t * a = (ssize_t *)malloc(sizeof(ssize_t) * nblocks);
  int i = 0;
  for (i = 0; i < nblocks; i++) {
    a[i] = i * block_sz;
  }
  if (mode[0] == 'r') {
    for (i = 0; i < 10 * nblocks; i++) {
      int j = rand() % nblocks;
      int k = rand() % nblocks;
      int t = a[j];
      a[j] = a[k];
      a[k] = t;
    }
  }
  return a;
}

int check_usage(int argc, char ** argv)
{
  if (argc == 1) {
    fprintf(stderr, "usage: %s filename [block_sz [n_times [s/r]]]\n",
	    argv[0]);
    exit(1);
  }
  return 0;
}

int main(int argc, char ** argv)
{
  /*  */
  int _ = check_usage(argc, argv);
  char * filename = argv[1];
  /* ファイルのサイズ */
  off_t sz = file_size(filename); 
  /* 一度に読むブロックサイズ */
  int block_sz = (argc > 2 ? atoi(argv[2]) : 1024);
  /* ファイルのブロック数 */
  int nblocks = (sz + block_sz - 1) / block_sz;	
  /* 同じファイルを何度読むか */
  int ntimes = (argc > 3 ? atoi(argv[3]) : 3);
  /*  */
  ssize_t * offsets = make_pointers(block_sz, nblocks, (argc > 4 ? argv[4] : "s"));
  char * buf = (char *)malloc(block_sz);
  block_record_t br = (block_record_t)calloc(sizeof(block_record), 
					     nblocks * ntimes);
  int __ = set_start_time();
  int i, j, k = 0;
  ssize_t bytes_read = 0;	/* 読んだ量 */
  double last = cur_time();
  for (i = 0; i < ntimes; i++) {
    fprintf(stderr, "%d-th scan\n", i);
    int sum = 0;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) { perror("open"); exit(1); }
    for (j = 0; j < nblocks; j++) {
      off_t o = lseek(fd, offsets[j], SEEK_SET);
      assert(o != -1);
      ssize_t m = read(fd, buf, block_sz);
      double t = cur_time();
      assert(m == block_sz);
      bytes_read += m;
      if (0) {
	int x;
	for (x = 0; x < block_sz; x++) sum += buf[x];
      }
      if (bytes_read % (1024 * 32) == 0) {
	/* 32KBに一回記録 */
	assert(k < nblocks * ntimes);
	br[k].t = t;
	br[k].bytes_read = bytes_read;
	k++;
      }
      last = t;
    }
    close(fd);
    if (i == 0) fprintf(stderr, "check sum = %d\n", sum);
  }
  {
    double t = cur_time();
    double mb = sz * ntimes / (1024.0 * 1024.0);
    fprintf(stderr, "%f MB in %f sec = %f MB/sec\n", 
	    mb, cur_time(), mb / cur_time());
  }
  {
    int i;
    for (i = 0; i < k; i++) {
      printf("%f %f\n", 
	     br[i].bytes_read / (1024.0 * 1024.0), br[i].t);
    }
  }
  return 0;
}
