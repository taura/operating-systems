#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

enum { n_chars_per_word = 10 };

int word_cmp(const void * s_, const void * t_) {
  const char * s = s_;
  const char * t = t_;
  size_t i;
  for (i = 0; i < n_chars_per_word; i++) {
    if (s[i] < t[i]) return -1;
    if (s[i] > t[i]) return  1;
  }
  return 0;
}

void random_word(char * word, size_t n_chars, unsigned short rg[3]) {
  size_t i;
  for (i = 0; i < n_chars; i++) {
    word[i] = 'a' + nrand48(rg) % ('z' - 'a');
  }
  word[n_chars - 1] = '\n';
}

int main(int argc, char ** argv) {
  if (argc <= 2) {
    fprintf(stderr, "usage: %s FILENAME N\n", argv[0]);
    exit(1);
  }
  char * filename = argv[1];
  size_t n = atol(argv[2]);
  size_t sz = n * n_chars_per_word;
  int fd = open(filename, O_RDWR|O_TRUNC|O_CREAT, 0644);
  if (fd == -1) { perror("open"); exit(1); }
  if (ftruncate(fd, sz) == -1) { perror("ftruncate"); exit(1); }
  char * a = mmap(0, sz, PROT_WRITE, MAP_SHARED, fd, 0);
  if (a == MAP_FAILED) { perror("mmap"); exit(1); }
  size_t i;
  unsigned short rg[3] = { 1, 2, 3 };
  for (i = 0; i < n; i++) {
    random_word(&a[i * n_chars_per_word], n_chars_per_word, rg);
  }
  qsort(a, n, n_chars_per_word, word_cmp);
  munmap(a, sz);
  close(fd);
  return 0;
}
