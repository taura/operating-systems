#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

void read_bytes(int fd, char * a, size_t sz) {
  size_t rd = 0;
  while (rd < sz) {
    ssize_t x = read(fd, a + rd, sz - rd);
    if (x == -1) { perror("read"); exit(1); }
    assert(x > 0);
    rd += x;
  }
}

int main(int argc, char ** argv) {
  if (argc <= 2) {
    fprintf(stderr, "usage: %s FILENAME N \n", argv[0]);
    exit(1);
  }
  char * filename = argv[1];
  ssize_t sz = atol(argv[2]);
  int fd = open(filename, O_RDONLY, 0644);
  if (fd == -1) { perror("open"); exit(1); }
  off_t real_sz = lseek(fd, 0, SEEK_END);
  if (real_sz == (off_t)-1) { perror("lseek"); exit(1); }
  if (lseek(fd, 0, SEEK_SET) != 0) { perror("lseek"); exit(1); }
  if (sz > real_sz) {
    fprintf(stderr, "file %s has only %ld (< %ld) bytes\n", 
	    filename, real_sz, sz);
    exit(1);
  }
  char * a = malloc(sz);
  if (!a) { perror("malloc"); exit(1); }
  read_bytes(fd, a, sz);



  return 0;
}
