/*
  dummy example to be debugged stepwise

  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Oct 31, 2002  By Pradeep Padala
 */

#include <stdio.h> /* printf() */
#include <stdlib.h>
#include <unistd.h> /* sleep() */

int main(int argc, char **argv)
{
  int max=10;
  int cnt;
  for (cnt = 0; cnt < max; ++cnt) {
    printf("rabbit (%d/%d)\n", cnt, max);
    sleep(3);
  }
  return 0;
}
