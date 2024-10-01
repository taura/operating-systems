#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
  pid_t pid = fork();
  if (pid == -1) {
    err(1, "fork");
  } else if (pid == 0) {          /* child */
    for (int i = 0; i < 5; i++) {
      printf("%d: %d\n", getpid(), i);
      fflush(stdout);
      usleep(100 * 1000);
    }
    return 123;
  } else {
    int ws;
    printf("parent: wait for child (pid = %d) to finish\n", pid);
    fflush(stdout);
    pid_t cid = waitpid(pid, &ws, 0);
    if (cid == -1) err(1, "waitpid");
    if (WIFEXITED(ws)) {
      printf("exited, status=%d\n", WEXITSTATUS(ws));
      fflush(stdout);
    } else if (WIFSIGNALED(ws)) {
      printf("killed by signal %d\n", WTERMSIG(ws));
      fflush(stdout);
    }
  }
  return 0;
}
