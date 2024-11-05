/*** com 1 */
#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int check_incore(char * filename) {
  /* ファイルを開く */
  int fd = open(filename, O_RDONLY);
  if (fd == -1) err(1, "open");
  /* ファイルのサイズ */
  struct stat sb[1];
  if (fstat(fd, sb) == -1) err(1, "fstat");
  /* mmap (読み出しのみ) */
  long sz = sb->st_size;
  char * a = mmap(0, sz, PROT_READ, MAP_SHARED, fd, 0);
  if (a == MAP_FAILED) err(1, "mmap");
  const long page_sz = 4096;
  long n_pages = (sz + page_sz - 1) / page_sz;
  unsigned char * incore = malloc(n_pages);
  memset(incore, 2, n_pages);

  if (mincore(a, n_pages * page_sz, incore) == -1) {
    err(1, "mincore");
  }
  long bytes_incore = 0;
  for (long i = 0; i < n_pages; i++) {
    assert(incore[i] == 0 || incore[i] == 1);
    if (incore[i]) bytes_incore += page_sz;
  }
  printf("%s : %ld bytes of %ld bytes on memory\n", filename, bytes_incore, sz);
  if (munmap(a, sz) == -1) err(1, "mumap");
  if (close(fd) == -1) err(1, "close");
  return 1;                     /* OK */
}

int main(int argc, char ** argv) {
  for (int i = 1; i < argc; i++) {
    check_incore(argv[i]);
  }
  return 0;
}
