#com 1
#include <assert.h>
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int drop_cache(const char * filename) {
  /* ファイルを開く */
  int fd = open(filename, O_RDWR, 0777);
  if (fd == -1) err(1, "open");
  struct stat sb[1];
  if (fstat(fd, sb) == -1) err(1, "fstat");
  if (fdatasync(fd) == -1) err(1, "fdatasync");
  long sz = sb->st_size;
  if (posix_fadvise(fd, 0, sz, POSIX_FADV_DONTNEED) == -1) {
    err(1, "posix_fadvise");
  }
  if (close(fd) == -1) err(1, "close");
  return 1;                     /* OK */
}

int main(int argc, char ** argv) {
  for (int i = 1; i < argc; i++) {
    drop_cache(argv[i]);
  }
  return 0;
}
