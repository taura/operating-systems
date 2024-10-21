#include <stdio.h>
#include <unistd.h>
#include <omp.h>

int main() {
  printf("hello\n");
#pragma omp parallel
  {
    /* 起動時に環境変数OMP_NUM_THREADS=xxx で指定した
       個数のスレッドが作られ, 各々が以下の文 { ... }
       を実行する.
       omp_get_num_threads() : { ... } を実行しているスレッド数を得る
       omp_get_thread_num() : その中での呼び出したスレッドの番号を得る
    */
    int idx = omp_get_thread_num();
    int nth = omp_get_num_threads();
    for (int i = 0; i < 4; i++) {
      usleep(1000);
      printf("hi I am %d of %d\n", idx, nth);
    }
  }
  printf("bye\n");
  return 0;
}
