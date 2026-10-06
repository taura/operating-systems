#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int x = 0;

/* スレッドの開始関数 */
void * f(void * arg) {
  x += 321;
  return 0;
}

int main() {
  x = 123;
  
  pid_t pid = fork();
  if (pid == -1) {
    err(1, "fork");
  } else if (pid == 0) {          /* child */
    f(0);
    return 0;
  } else {
    int ws;
    pid_t cid = waitpid(pid, &ws, 0);
    if (cid == -1) err(1, "waitpid");
    if (WIFEXITED(ws)) {
      printf("exited, status=%d\n", WEXITSTATUS(ws));
      fflush(stdout);
    } else if (WIFSIGNALED(ws)) {
      printf("killed by signal %d\n", WTERMSIG(ws));
      fflush(stdout);
    }
    printf("after the child finished, x = %d\n", x);
  }
  return 0;
}
