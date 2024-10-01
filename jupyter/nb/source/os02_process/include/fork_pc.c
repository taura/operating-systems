#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  pid_t pid = fork();           /* 現プロセスをコピー */
  if (pid == -1) {
    err(1, "fork");
  } else if (pid == 0) {        /* 新しいプロセス(子プロセス) */
    for (int i = 0; i < 5; i++) {
      printf("child  %d: %d\n", getpid(), i);
      fflush(stdout);
      usleep(100 * 1000);
    }
  } else {                      /* 元いたプロセス(親プロセス)
                                   forkの返り値は子プロセスのプロセスID */
    for (int i = 0; i < 5; i++) {
      printf("parent %d: %d\n", getpid(), i);
      fflush(stdout);
      usleep(100 * 1000);
    }
  }
  return 0;
}
