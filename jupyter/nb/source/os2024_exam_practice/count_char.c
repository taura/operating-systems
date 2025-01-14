/*** if VER == 1 */
#include <err.h>
/*** else */
#include <err.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*** endif */

#define BLK_SZ 4096

int main(int argc, char ** argv) {
  int idx = 1;
  char * file = (idx < argc ? argv[idx++] : "misc/wikipedia-operating-system.html");
  char c =      (idx < argc ? argv[idx++][0] : 'a');
/*** if VER == 1 */
/*** else */
  int fd = open(file, O_RDONLY);
/*** endif */
  if (fd == -1) err(1, "open");
  char a[BLK_SZ];
  ssize_t count = 0;
  while (1) {
/*** if VER == 1 */
/*** else */
    ssize_t rd = read(fd, a, BLK_SZ);
/*** endif */
    if (rd == -1) err(1, "read");
/*** if VER == 1 */
/*** else */
    if (rd == 0) break;
/*** endif */
    for (ssize_t i = 0; i < rd; i++) {
      if (a[i] == c) count++;
    }
  }
  printf("%ld\n", count);
  return 0;
}
