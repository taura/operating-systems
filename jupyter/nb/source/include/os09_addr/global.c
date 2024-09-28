#com 2
#include <stdio.h>
#ifpy VER == 1
/* 大域変数 */
int x;
/* 大域配列 */
int c[100];
double d[200];
/* 大域変数 */
int i;

int main() {
  printf("x:     %ld\n", &x);
  printf("c:     %ld\n", c);
  printf("c[50]: %ld\n", &c[50]);
  printf("d:     %ld\n", d);
  printf("d[50]: %ld\n", &d[50]);
  printf("i:     %ld\n", &i);
}
#elifpy VER == 2
#include <stdio.h>
/* 大域配列 */
int c[100];
/* 大域変数i */
int i;

int main() {
  for (i = 0; i <= 100; i++) {
    c[i] = 0;
  }
  printf("done\n");
}
#endifpy
