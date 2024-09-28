#com 6

#ifpy VER in [1, 3, 5]
int main() {



  return 0;
}
#endifpy
#ifpy VER == 2
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
  int key         = (argc > i ? atoi(argv[i]) : 100); i++;
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
  int * found = bsearch(&key, a, n, sizeof(int), compare_int);
  if (found) {
    printf("%d found at %ld-th element\n", key, found - a);
  } else {
    printf("%d not found\n", key);
  }
  if (munmap(a, sz) == -1) err(1, "mumap");
  if (close(fd) == -1) err(1, "close");
  return 0;
}
#endifpy
#ifpy VER == 4
#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int compare_int(const void * a_, const void * b_) {
  const int * a = a_;
  const int * b = b_;
  return *a - *b;
}

/* szバイトぴったり読む. その前にEOFになったらエラー */
void read_sz(int fd, void * buf, size_t sz) {
  size_t rd = 0;
  while (rd < sz) {
    ssize_t x = read(fd, buf + rd, sz - rd);
    if (x == -1) err(1, "read");
    assert(x > 0);
    rd += x;
  }
  assert(rd == sz);
}

int main(int argc, char ** argv) {
  int i = 1;
  char * filename = (argc > i ? argv[i] : "sorted.bin"); i++;
  int key         = (argc > i ? atoi(argv[i]) : 100); i++;
  /* ファイルを開く */
  int fd = open(filename, O_RDONLY);
  if (fd == -1) err(1, "open");
  /* ファイルのサイズ */
  struct stat sb[1];
  if (fstat(fd, sb) == -1) err(1, "fstat");
  /* mmap (読み出しのみ) */
  long sz = sb->st_size;
  int * a = malloc(sz);
  if (!a) err(1, "malloc");
  read_sz(fd, a, sz);
  const long n = sz / sizeof(int);
  int * found = bsearch(&key, a, n, sizeof(int), compare_int);
  if (found) {
    printf("%d found at %ld-th element\n", key, found - a);
  } else {
    printf("%d not found\n", key);
  }
  free(a);
  if (close(fd) == -1) err(1, "close");
  return 0;
}
#endifpy
#ifpy VER == 6
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

/* szバイトぴったり読む. その前にEOFになったらエラー */
void read_sz(int fd, void * buf, size_t sz) {
  size_t rd = 0;
  while (rd < sz) {
    ssize_t x = read(fd, buf + rd, sz - rd);
    if (x == -1) err(1, "read");
    assert(x > 0);
    rd += x;
  }
  assert(rd == sz);
}

int main(int argc, char ** argv) {
  int i = 1;
  char * filename = (argc > i ? argv[i] : "sorted.bin"); i++;
  int key         = (argc > i ? atoi(argv[i]) : 100); i++;
  /* ファイルを開く */
  int fd = open(filename, O_RDONLY);
  if (fd == -1) err(1, "open");
  /* ファイルのサイズ */
  struct stat sb[1];
  if (fstat(fd, sb) == -1) err(1, "fstat");
  /* mmap (読み出しのみ) */
  long sz = sb->st_size;
  const long n = sz / sizeof(int);
  const long buf_sz = 1024;
  int a[buf_sz];
  int done = 0;
  for (long i = 0; i < n && !done; i += buf_sz) {
    long m = (buf_sz < n - i ? buf_sz : n - i);
    read_sz(fd, a, m * sizeof(int));
    for (long j = 0; j < m; j++) {
      if (a[j] == key) {
        done = 1;
        printf("%d found at %ld-th element\n", key, i + j);
        break;
      } else if (a[j] > key) {
        done = 1;
        printf("%d not found\n", key);
        break;
      }
    }
  }
  if (!done) {
    printf("%d not found\n", key);
  }
  if (close(fd) == -1) err(1, "close");
  return 0;
}
#endifpy
