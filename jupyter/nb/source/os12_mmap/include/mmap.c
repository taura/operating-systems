#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/*** if VER == 1 */
int main(int argc, char ** argv) {
  int i = 1;
  char * filename = (argc > i ? argv[i] : "mmap_1.c"); i++;
  /* ファイルを開く */
  int fd = open(filename, O_RDONLY);
  if (fd == -1) err(1, "open");
  /* ファイルのサイズ */
  struct stat sb[1];
  if (fstat(fd, sb) == -1) err(1, "fstat");
  /* mmap (読み出しのみ) */
  long sz = sb->st_size;
  char * a = mmap(0, sz, PROT_READ, MAP_PRIVATE, fd, 0);
  if (a == MAP_FAILED) err(1, "mmap");
  /* 配列aを表示すると filename の中身を表示することになる*/
  for (long i = 0; i < sz; i++) {
    putchar(a[i]);
  }
  if (munmap(a, sz) == -1) err(1, "mumap");
  if (close(fd) == -1) err(1, "close");
  return 0;
}
/*** endif */
/*** if VER == 2 */
int main(int argc, char ** argv) {
  int i = 1;
  char * filename = (argc > i ? argv[i] : "hogehoge.txt"); i++;
  long sz         = (argc > i ? atol(argv[i]) : 1000); i++;
  /* ファイルを開く */
  int fd = open(filename, O_RDWR|O_TRUNC|O_CREAT, 0777);
  if (fd == -1) err(1, "open");
  if (posix_fallocate(fd, 0, sz) == -1) err(1, "posix_fallocate");
  /* mmap */
  char * a = mmap(0, sz, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  if (a == MAP_FAILED) err(1, "mmap");
  /* 配列aに書き込むとfilenameに書き込むことになる */
  for (long i = 0; i < sz; i++) {
    a[i] = i % 128;
  }
  if (munmap(a, sz) == -1) err(1, "mumap");
  if (close(fd) == -1) err(1, "close");
  return 0;
}
/*** endif */
/*** if VER == 3 */
int main(int argc, char ** argv) {
  int i = 1;
  long sz         = (argc > i ? atol(argv[i]) : 1000); i++;
  /* mmap メモリ割当のみ */
  char * a = mmap(0, sz, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
  if (a == MAP_FAILED) err(1, "mmap");
  /* これは単なるメモリ割り当て */
  for (long i = 0; i < sz; i++) {
    assert(a[i] == 0);
  }
  if (munmap(a, sz) == -1) err(1, "mumap");
  printf("OK\n");
  return 0;
}
/*** endif */
