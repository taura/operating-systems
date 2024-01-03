#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int compare_int(const void * a_, const void * b_) {
  const int * a = a_;
  const int * b = b_;
  return *a - *b;
}

int main(int argc, char ** argv) {
  int i = 1;
  char * filename = (argc > i ? argv[i] : "sorted.bin"); i++;
  int found       = (argc > i ? atoi(argv[i]) : 1); i++;
  long pos        = (argc > i ? atol(argv[i]) : -1); i++;
  long seed       = (argc > i ? atol(argv[i]) : 123456789012345); i++;
  unsigned short rg[3] = {
    (seed >> 32) & 0xFFFF,
    (seed >> 16) & 0xFFFF,
    (seed >>  0) & 0xFFFF,
  };      
  /* ファイルを開く */
  int fd = open(filename, O_RDONLY);
  if (fd == -1) err(1, "open");
  /* ファイルのサイズ */
  struct stat sb[1];
  if (fstat(fd, sb) == -1) err(1, "fstat");
  /* mmap (読み出しのみ) */
  long sz = sb->st_size;
  int * a = mmap(0, sz, PROT_READ, MAP_PRIVATE, fd, 0);
  if (a == MAP_FAILED) err(1, "mmap");
  const long n = sz / sizeof(int);
  if (pos == -1) {
    pos = n * erand48(rg);
  }
  if (found) {
    while (pos > 0 && a[pos - 1] == a[pos]) {
      pos--;
    }
    printf("if /usr/bin/time ./mmap_bsearch ${data} %d | grep \"%d found at %ld-th element\" ; then echo OK ; else echo NG ; fi\n",
           a[pos], a[pos], pos);
  } else {
    while (pos < n && a[pos + 1] <= a[pos] + 1) {
      pos++;
    }
    /* pos == n || a[pos + 1] > a[pos] + 1 */
    printf("if /usr/bin/time ./mmap_bsearch ${data} %d | grep \"%d not found\" ; then echo OK ; else echo NG ; fi\n",
           a[pos] + 1, a[pos] + 1);
  }
  /* 
     if /usr/bin/time ./mmap_bsearch ${data} 26127088  | grep "26127088 found at 7466042-th element" ; then echo OK ; else echo NG ; fi
 */
  if (munmap(a, sz) == -1) err(1, "mumap");
  if (close(fd) == -1) err(1, "close");
  return 0;
}
