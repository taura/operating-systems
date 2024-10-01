#include <stdio.h>
#include <unistd.h>

int main() {
  printf("%d : before fork\n", getpid());
  fflush(stdout);
  fork();           /* 現プロセスをコピー */
  printf("%d : after fork\n", getpid());
  return 0;
}
