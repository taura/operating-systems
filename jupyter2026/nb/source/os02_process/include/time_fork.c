#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

long cur_time() {
  struct timespec ts[1];
  clock_gettime(CLOCK_REALTIME, ts);
  return ts->tv_sec * 1000000000L + ts->tv_nsec;
}

int main(int argc, char ** argv) {
  int n = (argc > 1 ? atoi(argv[1]) : 5);
  long t0 = cur_time();
/*** if VER % 2 == 1 */

  
  ここにプログラムを書く

  
/*** else */
  for (int i = 0; i < n; i++) {
    pid_t pid = fork();           /* 現プロセスをコピー */
    if (pid == -1) {
      err(1, "fork");
    } else if (pid == 0) {        /* 新しいプロセス(子プロセス) */
/*** if VER == 2 */
      char * const argv[] = { "./do_nothing", 0 };
      execvp(argv[0], argv);
      err(1, "execve");
/*** elif VER == 4 */
      exit(0);
/*** endif */
    } else {
      int ws;
      pid_t cid = waitpid(pid, &ws, 0);
      if (cid == -1) err(1, "waitpid");
    }
  }
/*** endif */
  long t1 = cur_time();
  long dt = t1 - t0;
/*** if VER <= 2 */
  printf("%ld nsec to fork-exec-wait %d processes (%ld nsec/proc)\n",
         dt, n, dt / n);
/*** elif VER <= 4 */
  printf("%ld nsec to fork-wait %d processes (%ld nsec/proc)\n",
         dt, n, dt / n);
/*** endif */
  return 0;
}
