#com 1
//% file: %%BASENAME%%.c
//% cmd: gcc -Wall -Wextra -o %%BASENAME%% %%BASENAME%%.c

#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

volatile long n_sigints = 0;

void sigint_action(int sig, siginfo_t * info, void * arg) {
  (void)sig; (void)info; (void)arg;
  printf("got sigaction %ld\n", n_sigints);
  fflush(stdout);
  n_sigints++;
}

int main() {
  struct sigaction act;
  act.sa_sigaction = sigint_action;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  if (sigaction(SIGINT, &act, 0) == -1) err(1, "sigaction");
  pid_t pid = getpid();
  printf("please send me (pid = %d) SIGINT signal 5 times\n", pid);
  fflush(stdout);
  while (n_sigints < 5) { }
  return 0;
}
