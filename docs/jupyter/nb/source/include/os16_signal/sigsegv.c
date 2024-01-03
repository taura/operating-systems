#com 1
//% file: %%BASENAME%%.c
//% cmd: gcc -Wall -Wextra -o %%BASENAME%% %%BASENAME%%.c

#include <assert.h>
#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

volatile long n_sigints = 0;

void sigsegv_action(int sig, siginfo_t * sinfo, void * arg) {
  (void)arg;
  void * addr = sinfo->si_addr;
  size_t page_sz = 4096;
  assert(sig == SIGSEGV);
  printf("got segv at %p\n", addr);
  fflush(stdout);
  void * page_addr = (void*)((long)addr & ~(page_sz - 1));
  if (mprotect(page_addr, page_sz, PROT_READ|PROT_WRITE) == -1)
    err(1, "mprotect");
}

int main() {
  struct sigaction act;
  act.sa_sigaction = sigsegv_action;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  if (sigaction(SIGSEGV, &act, 0) == -1) err(1, "sigaction");
  size_t page_sz = 4096;
  size_t sz = 4096;
  char * a = aligned_alloc(page_sz, sz);
  if (!a) err(1, "aligned_alloc");
  printf("a = %p\n", a);
  printf("a[0] = 100\n");
  a[0] = 100;
  if (mprotect(a, sz, PROT_READ) == -1) err(1, "mprotect");
  printf("a[0] = 200\n");
  a[0] = 200;
  printf("OK\n");
  return 0;
}
